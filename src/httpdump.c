#include "config.h"
#include <ctype.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <apr_hash.h>
#include <apr_tables.h>
#include <apr_strings.h>
#include <regex.h>

#define __FAVOR_BSD 1 /* to support compilation on OS X */
#include <netinet/tcp.h>

#include "http_conversation.h"
#include "parser.h"
#include "json_format.h"
#include "xml_format.h"

#define FILTER "tcp port 80 or 8080"
#ifndef PCAP_NETMASK_UNKNOWN
#define PCAP_NETMASK_UNKNOWN 0xffffffff
#endif
enum {
    MAX_PACKET = 65535,
};
typedef enum {
    TEXT = 0,
    JSON = 1,
    XML = 2,
} format_t;
    
    
struct params {
    int offset;
    apr_hash_t *conversations;
    int count;
    int max_count;
    int snaplen;
    pcap_t *pcap;
    regex_t regex;
    format_t format;
} params;

static int verbose = 0;

static void
print_data(tcp_stream_t *t, int snaplen)
{
    int print, i;

    print = t->length < params.snaplen ? t->length : snaplen;
    for (i = 0; i < print; i++) 
        printf("%c", (isprint(t->data[i]) || isspace(t->data[i])) ? t->data[i] : '.');
}

static void
print_text(http_conversation_t *conversation) 
{
    printf("# %d.%d.%d.%d:%d <-> %d.%d.%d.%d:%d\n",
            conversation->key.src >> 24,
            (conversation->key.src & 0xff0000) >> 16,
            (conversation->key.src & 0xff00) >> 8,
            conversation->key.src & 0xff,
            conversation->key.s_port, 
            conversation->key.dst >> 24,
            (conversation->key.dst & 0xff0000) >> 16,
            (conversation->key.dst & 0xff00) >> 8,
            conversation->key.dst & 0xff,
            conversation->key.d_port);
    
#ifdef DARWIN
    printf("# %lu.%06u %d\n", conversation->request.ts.tv_sec, conversation->request.ts.tv_usec, conversation->request.length);
#else
    printf("# %lu.%06lu %d\n", conversation->request.ts.tv_sec, conversation->request.ts.tv_usec, conversation->request.length);
#endif
    print_data(&conversation->request, params.snaplen);
    printf("\r\n");
    
#ifdef DARWIN
    printf("# %lu.%06u %d\n", conversation->reply.ts.tv_sec, conversation->reply.ts.tv_usec, conversation->reply.length);
#else
    printf("# %lu.%06lu %d\n", conversation->reply.ts.tv_sec, conversation->reply.ts.tv_usec, conversation->reply.length);
#endif
    print_data(&conversation->reply, params.snaplen);
    printf("\r\n");
}

/*
 * the callback is invoked in time order of the packets ...
 */
static void
callback(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes) 
{
    struct params *params = (struct params *)user;
    apr_hash_t *conversations = params->conversations;
    int offset = params->offset;
    apr_pool_t *pool = apr_hash_pool_get(conversations);
    const struct ip *ip = (const struct ip *)(bytes + offset);
    const struct tcphdr *tcp
        = (const struct tcphdr *)((char *)ip + sizeof(struct ip));
    u_int pkt_len = h->len;
    u_int ip_len = 0;
    u_int payload_len = 0;
    char *payload;
    int i;
    hash_key_t key, rev_key;
    tcp_stream_t *tcp_stream = NULL;
    http_conversation_t *conversation = NULL;
    
    ip_len = ntohs(ip->ip_len);

    if (ip->ip_v != 4 || ip->ip_hl < 5 || pkt_len < ip_len
        || ip->ip_off & IP_MF) {
        fprintf(stderr, "Bad IP packet v: %d h: %d: l p/i: %d/%d f: %x\n",
                ip->ip_v, ip->ip_hl, pkt_len, ip_len, ip->ip_off);
        return;
    }


    key.src = ntohl(ip->ip_src.s_addr);
    key.dst = ntohl(ip->ip_dst.s_addr);
    key.s_port = ntohs(tcp->th_sport);
    key.d_port = ntohs(tcp->th_dport);
    rev_key.src = key.dst;
    rev_key.dst = key.src;
    rev_key.s_port = key.d_port;
    rev_key.d_port = key.s_port;


    if ((conversation = apr_hash_get(conversations, &key, sizeof(key))))
        tcp_stream = &conversation->request;
    else if ((conversation = apr_hash_get(conversations, &rev_key,
                                         sizeof(rev_key)))) 
        tcp_stream = &conversation->reply;
    else if ((tcp->th_flags & TH_SYN) && !(tcp->th_flags & TH_ACK)) {
        conversation = calloc(1, sizeof(*conversation));
        conversation->key = key;
        
        apr_hash_set(conversations, &conversation->key,
                     sizeof(key), conversation);
        tcp_stream = &conversation->request;
    } else 
        return;

    payload_len = ip_len - ((ip->ip_hl + tcp->th_off) << 2);

    if (tcp_stream->ts.tv_sec == 0)
        tcp_stream->ts = h->ts;
  
    
    /* flush when we see the FIN flag */
    if (tcp->th_flags & TH_FIN) {
        conversation->fins++;
        if (2 == conversation->fins) {
            if ((conversation->request.data &&
                 !regexec(&params->regex, conversation->request.data, 0, NULL, 0))
                || (conversation->reply.data &&
                    !regexec(&params->regex, conversation->reply.data, 0, NULL, 0))) {
                params->count++;
                switch (params->format) {
                case JSON:
                    if (params->count > 1)
                        printf("  ,\n");
                    print_json(conversation);
                    break;
                case XML:
                    parse_conversation(conversation);
                    print_parsed_conversation(conversation);
                    break;
                default:
                    if (params->count > 1)
                        printf("\n");
                    print_text(conversation);
                    break;
                }
            } 
            
            apr_hash_set(conversations, &conversation->key,
                         sizeof(key), NULL);
            free(conversation->request.data);
            
            free(conversation->reply.data);

            if (conversation->requests)
                free(conversation->requests);
            if (conversation->replies)
                free(conversation->replies);
            
            free(conversation);
            if (params->max_count > 0 && params->count >= params->max_count)
                pcap_breakloop(params->pcap);
            return;
        }
        
    }

    if (0 == payload_len || !tcp_stream)
        return;


    payload = tcp_stream->data;
    
    tcp_stream->data = realloc(payload, tcp_stream->length + payload_len + 1);
    if (!tcp_stream->data) {
        fprintf(stderr, "not able to allocate memory ...\n");
        free(payload);
        exit(-4);
    }
    
    memmove(tcp_stream->data + tcp_stream->length,
           (char *)tcp + tcp->th_off * 4, payload_len);
    tcp_stream->length += payload_len;
    tcp_stream->data[tcp_stream->length] = '\0';
}

void
usage(const char *progname)
{
    fprintf(stdout, "Usage: %s [-DhpV] [ -c count ] [-g regex] [ -i interface ][-r file] [-s snaplen] [-w file] [tcpdump filter (%s)]\n",
            progname, FILTER);
    fprintf(stdout, "\tRe-assembles HTTP conversation.\n\n");
    fprintf(stdout, "\t-c Exit after receiving 'count' conversations.\n");
    fprintf(stdout, "\t-D Print the list of the network interfaces available for capturing conversations.\n");
    fprintf(stdout, "\t-g Only show conversations that match 'regex'.\n");
    fprintf(stdout, "\t-i Listen on 'interface'.\n");
    fprintf(stdout, "\t-J Print conversations in JSON.\n");
    fprintf(stdout, "\t-p Don't put the interface into promiscous mode.\n");
    fprintf(stdout, "\t-r Read packets from 'file'. Standard input is used if file is ``-''.\n");
    fprintf(stdout, "\t-s Snarf 'snaplen' bytes of data from each packet rather than the default %d.\n", MAX_PACKET);
    fprintf(stdout, "\t-V verbose output.\n");
    fprintf(stdout, "\t-w Write the conversations to 'file' rather than printing them out.\n");
    fprintf(stdout, "\t-X Print conversations in XML.\n");
    fflush(stdout);
}

int
main (int argc, char *argv[]) 
{
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap = NULL;
    struct pcap_pkthdr *pkt_header;
    const u_char *pkt_data;
    struct bpf_program fp;
    char *filter = FILTER;
    apr_pool_t *pool = NULL;
    int c, i;
    struct pcap_file_header header;
    apr_array_header_t *http_conversations;
    int fd = 0;
    const char *interface = NULL;
    int promisc = 1;

    params.snaplen = MAX_PACKET;
    regcomp(&params.regex, ".*", REG_EXTENDED);
    
    while (-1 != (c = getopt(argc, argv, "hH?c:Dg:i:Jpr:s:Vvw:X")))  {
        switch (c) {
        case 'c':
            params.max_count = atoi(optarg);
            break;
        case 'D': {
            pcap_if_t *pcap_if = NULL;
            pcap_if_t *p;
            int i = 1;
            
            if (-1 == pcap_findalldevs(&pcap_if, errbuf)) {
                fprintf(stderr, "Cannot list devices: %s\n", errbuf);
                return -1;
            }
            for (p = pcap_if; p; p = p->next) {    
                fprintf(stdout, "%d.%s", i++, p->name);
                if (p->description)
                    fprintf(stdout, " (%s)\n",p->description);
                else
                    fprintf(stdout, "\n");
            }
            
            if (pcap_if)
                pcap_freealldevs(pcap_if);
            return 0;
        }
            break;
        case 'g': {
            int reg_err
                = regcomp(&params.regex, optarg, REG_EXTENDED|REG_NOSUB);
            
            if (reg_err) {
                size_t length = regerror (reg_err, &params.regex, NULL, 0);
                char *buffer = malloc (length);
                regerror(reg_err, &params.regex, buffer, length);
                fprintf(stderr, "Cannot compile regex '%s': %s\n", optarg, buffer);
                exit (-1);
            }
        }
        
            break;
        case 'h':
        case '?':
            usage(argv[0]);
            exit(-1);
        case 'i':
            interface = optarg;
            break;
        case 'J':
            params.format = JSON;
            break;
        case 'p':
            promisc = 0;
            break;
        case 'r':
            pcap = pcap_open_offline(optarg, errbuf);
            if (!pcap) {
                fprintf(stderr, "Could not open %s: %s\n", optarg, errbuf);
                exit(-2);
            }
            break;
        case 's':
            params.snaplen = atoi(optarg);
            break;
        case 'V':
            verbose = 1;
            break;
        case 'v':
            fprintf(stdout, "%s\n\nWritten by <%s>\n", PACKAGE_STRING, PACKAGE_BUGREPORT);
            return 0;
        case 'w':
            fd = open(optarg, O_CREAT|O_TRUNC|O_RDWR, 0644);
            dup2(fd, STDOUT_FILENO);
            break;
        case 'X':
            params.format = XML;
            break;
        default:
            usage(argv[0]);
            return 0;
        }
    }

    if (optind < argc) {
        int f_len = 1;
        filter = calloc(1, sizeof(char));
        
        for (i = optind; i < argc; i++) {
            filter = realloc(filter, strlen(argv[i]) + 1 + f_len);
            strcat(filter, argv[i]);
            strcat(filter, " ");
            f_len = strlen(filter);
        }
    }
    
    if (!pcap) {
        if (!interface)
            interface = pcap_lookupdev(errbuf);
        
        if (NULL == interface) {
            fprintf(stderr, "Could not find default device %s: %s\n",
                    interface, errbuf);
            exit(-3);
        }
        pcap = pcap_open_live(interface, params.snaplen, promisc, 1000, errbuf);
        if (!pcap) {
            fprintf(stderr, "Could not open %s: %s\n", interface, errbuf);
            exit(-2);
        }
    }
    
    if (-1 == pcap_compile(pcap, &fp, filter, 1, PCAP_NETMASK_UNKNOWN)) {
        fprintf(stderr, "Could not compile filter '%s': %s\n",
                filter, pcap_geterr(pcap));
        exit(-2);
    }
    
    if (-1 == pcap_setfilter(pcap, &fp)) {
        fprintf(stderr, "Could not set filter %s: %s\n",
                filter, pcap_geterr(pcap));
        exit(-2);
    }
    
    switch (pcap_datalink(pcap)) {
    case DLT_LINUX_SLL:
        params.offset = 16;
        break;
    case DLT_EN10MB:
        params.offset = sizeof(struct ether_header);
        break;
    default:
        params.offset = 0;
    }

    apr_app_initialize(&argc, (const char *const **)&argv, NULL);
    atexit(apr_terminate);
    apr_pool_create(&pool, NULL);
    params.conversations = apr_hash_make(pool);
    params.pcap = pcap;

    if (params.format == JSON)
        printf("{\n  \"conversations\": [\n");
    pcap_loop(pcap, -1, callback, (u_char*)&params);
    if (params.format == JSON)
        printf("  ]\n}\n");

    regfree(&params.regex);
    pcap_freecode(&fp);
    
    pcap_close(pcap);
    if (fd)
        close(fd);
    return 0;
    
}

/* Local Variables: */
/* mode:c */
/* compile-command: "gcc -O0 -g3 -I.. -I/usr/include/apr-1.0 -I/usr/include/apr-1  -I/opt/local/include/apr-1 -I/usr/include/pcap -I../include -o httpdump httpdump.c parser.c json_format.c xml_format.c -lpcap -lapr-1 -laprutil-1"  */
/* End: */

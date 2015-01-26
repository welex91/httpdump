/* parse an http_conversation from char stream to a structure */
#include <stdio.h>
#include <string.h>
#include "http_conversation.h"

static char *
trim(char *c)
{
    while(c && isspace(*c))
        c++;
    return c;
}

message_t *
gather(message_t *m, int count) 
{
    int i;
    int h_total;
    message_t *gather_m;
    header_t *headers;
    
    for (i = 0, h_total = 0; i < count; i++) 
        h_total += m[i].h_count;
    
    gather_m = malloc(count * sizeof(*gather_m) + h_total * sizeof(header_t));
    headers = (header_t *)(gather_m + count);
    
    for (i = 0; i < count; i++) {
        memcpy(gather_m+i, m+i, sizeof(m[i]));
        gather_m[i].headers = headers;
        memcpy(headers, m[i].headers, m[i].h_count * sizeof(header_t));
        headers += m[i].h_count;
    }
    return gather_m;
}

int
parse_headers(message_t *m, char *s) 
{
    int h = 0;
    int length = 0;
    char *p;
    char *c;
    
    while (NULL != (c = strtok_r(s, ":", &p))) {
        s = NULL;
        m->headers
            = realloc(m->headers,
                      (h + 1) * sizeof(*m->headers));
        m->headers[h].name = trim(c);
        c = strtok_r(NULL, "\r\n", &p);
        m->headers[h].value = trim(c);
        
        if (!strcasecmp("Content-Length", m->headers[h].name)) 
            length = atoi(m->headers[h].value);
        
        h++;
    }
    m->h_count = h;

    return length;
}

int
parse_message(message_t *m, char *s) 
{
    char *p, *e, *c;
    int reply = 0;

    /* look for the end of the request/status line */
    e = strstr(s, "\n");
    if (e)
        *e = '\0';
    else
        return -1;
    
    /* look for the beginning of the request/status line */
    c = strtok_r(s, " ", &p);    
    if (!c)
        return -1;
    
    /* check for request line */
    if (!strcmp("GET", c))
        m->method = GET;
    else if (!strcmp("POST", c))
        m->method = POST;
    /* TODO: add other legal methods */
    
    if (INVALID == m->method) {
        m->version = ZERO_NINE;

        /* could be a reply status line */
        if (!strcmp("HTTP/1.0", c)) 
            m->version = ONE_ZERO;
        else if (!strcmp("HTTP/1.1", c))
            m->version = ONE_ONE;
        
        /* TODO: grab simple reply */

        if (ZERO_NINE != m->version) {
            c = strtok_r(NULL, " ", &p);
            m->status = atoi(c);

            c = strtok_r(NULL, "\r\n", &p);
            m->reason = c;
        }        
    } else {
        char *t;
        
        c = strtok_r(NULL, " ", &p);
        m->url = c;
        
        if (strtok_r(m->url, "?;", &t))
            m->query_string = strtok_r(NULL, "?;", &t);        
        
        c = strtok_r(NULL, "\r\n", &p);
        if (!strcmp("HTTP/1.0", c))
            m->version = ONE_ZERO;
        else if (!strcmp("HTTP/1.1", c))
            m->version = ONE_ONE;
        else
            m->version = ZERO_NINE;
    }

    m->length = parse_headers(m, e + 1);

    /* the HTTP body is not necessarily '\0' terminated ! */
    if (m->length > 0) {
        m->body = e + strlen("\r\n\r\n");
        c = m->body + m->length;
    } else if (e)
        c = e + strlen("\r\n\r\n");
    
    return c - s;
}

int
parse_conversation(http_conversation_t *conversation)
{
    int i = 0, h;
    char *p, *c, *e;
    message_t *requests = NULL;
    message_t *replies = NULL;
    
    if (conversation->requests)
        free(conversation->requests);
    if (conversation->replies)
        free(conversation->replies);

    c = conversation->request.data;
    while (NULL != c
           && c < conversation->request.data + conversation->request.length) {
        int parsed = 0;
        
        requests = realloc(requests, (i + 1) * sizeof(*requests));
        requests[i].headers = requests[i].body = NULL;
        requests[i].method = INVALID;

        parsed = parse_message(requests + i, c);
        if (parsed < 0)
            return -1;
        c += parsed;
        i++;
    }
    i = 0;

    /* parse replies */
    c = conversation->reply.data;
    while (NULL != c
           && c < conversation->reply.data + conversation->reply.length) {
        replies = realloc(replies, (i + 1) * sizeof(*replies));
        replies[i].headers = replies[i].body = replies[i].url = NULL;
        replies[i].method = INVALID;
        
        c += parse_message(replies + i, c);
        i++;
    }
    conversation->r_count = i;

    /* do a single block memory allocation */
    conversation->requests = gather(requests, conversation->r_count);
    free(requests->headers);
    free(requests);
    
    conversation->replies = gather(replies, conversation->r_count);
    free(replies->headers);
    free(replies);

    return 0;
}

void
print_parsed_conversation(http_conversation_t *conversation)
{
    const char *method[] = {"OPTIONS", "GET", "HEAD", "POST",
                            "PUT", "DELETE", "TRACE", "CONNECT"};
    const char *version[] = {"1.0", "1.1"};
    message_t *requests = conversation->requests;
    message_t *replies = conversation->replies;
    int i, h;

    for (i = 0; i < conversation->r_count; i++) {
        printf("%s %s%s%s HTTP/%s\n",
               method[requests[i].method],
               requests[i].url,
               requests[i].query_string ? "?" : "",
               requests[i].query_string ? requests[i].query_string : "",
               version[requests[i].version]);

        for (h = 0; h < requests[i].h_count; h++) 
            printf("%s: %s\r\n", requests[i].headers[h].name,
                   requests[i].headers[h].value);
        printf("\n%s\n\n", requests[i].body ? requests[i].body : "");
        
        printf("HTTP/%s %3d %s\n",
               version[replies[i].version],
               replies[i].status,
               replies[i].reason);

        for (h = 0; h < replies[i].h_count; h++) 
            printf("%s: %s\r\n", replies[i].headers[h].name,
                   replies[i].headers[h].value);
        printf("\n%s\n\n", replies[i].body ? replies[i].body : "");
    }
}


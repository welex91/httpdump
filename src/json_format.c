#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "http_conversation.h"
#include "json_format.h"
#include "parser.h"

static void
print_data(message_t *m, int count)
{
    const char *version[] = {"0.9", "1.0", "1.1"};
    const char *method[] = {"OPTIONS", "GET", "HEAD", "POST",
                            "PUT", "DELETE", "TRACE", "CONNECT"};

    int i, j;
    
    for (i = 0; i < count; i++) {
        if (m[i].url) {
            printf("      \"method\": \"%s\"\n", method[m[i].method]);
            printf("      \"uri\": \"%s", m[i].url);
            if (m[i].query_string)
                printf("?%s", m[i].query_string);
            printf("\"\n");
        } else {
            printf("      \"status\": %d\n", m[i].status);
            printf("      \"reason\": \"%s\"\n", m[i].reason);
        }
        
        printf("      \"version\": %s\n", version[m[i].version]);
        printf("      \"headers\": [\n");
        for (j = 0; j < m[i].h_count; j++)
            printf("         { \"name\": \"%s\", \"value\": \"%s\"},\n",
                   m[i].headers[j].name,
                   m[i].headers[j].value);
        printf("      ]\n");
        if (m[i].body) {
            printf("      \"body\": \"");
            for (j = 0; j < m[i].length; j++) {
                char c = m[i].body[j];
                
                switch (c) {
                case '"':
                case '\\': 
/*        case '/': */
                    printf("\\\%c", c);
                    break;
                case '\b':
                    printf("\\b");
                    break;
                case '\f':
                    printf("\\f");
                    break;
                case '\n':
                    printf("\\n");
                    break;
                case '\r':
                    printf("\\r");
                    break;
                case '\t':            
                    printf("\\t");
                    break;
                default:
                    printf("%c", (isprint(c) || isspace(c)) ? c : '.');
                }
            }
            printf("\"\n");
        }
    }
    
}

void
print_json(http_conversation_t *conversation) 
{
    if (!conversation->requests && parse_conversation(conversation))
        return;
    
    printf("  {\n");
    printf("    \"source\": {");
    printf("\"ip\": \"%d.%d.%d.%d\", ",
            conversation->key.src >> 24,
            (conversation->key.src & 0xff0000) >> 16,
            (conversation->key.src & 0xff00) >> 8,
            conversation->key.src & 0xff);
    printf("\"port\": %d},\n", conversation->key.s_port);
    printf("    \"dest\": {");
    printf("\"ip\": \"%d.%d.%d.%d\", ",
            conversation->key.dst >> 24,
            (conversation->key.dst & 0xff0000) >> 16,
            (conversation->key.dst & 0xff00) >> 8,
            conversation->key.dst & 0xff);
    printf("\"port\": %d},\n", conversation->key.d_port);
    printf("    \"request\": {\n");
    printf("      \"timestamp\": %lu.%06u,\n",
            conversation->request.ts.tv_sec,
            conversation->request.ts.tv_usec);
    printf("      \"length\": %d,\n", conversation->request.length);
    print_data(conversation->requests, conversation->r_count);
    printf("    },\n");
    printf("    \"reply\": {\n");
    printf("      \"timestamp\": %lu.%06u,\n",
            conversation->reply.ts.tv_sec,
            conversation->reply.ts.tv_usec);
    printf("      \"length\": %d,\n", conversation->reply.length);
    print_data(conversation->replies, conversation->r_count);
    printf("    } \n");
    printf("  }");    
}


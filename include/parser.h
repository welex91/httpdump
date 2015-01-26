#ifndef _PARSER_H
#define _PARSER_H
int parse_conversation(http_conversation_t *conversation);
int print_parsed_conversation(http_conversation_t *conversation);
message_t *gather(message_t *m, int count);
#endif /* _PARSER_H */

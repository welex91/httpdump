#ifndef _HTTP_CONVERSATION_H
#define _HTTP_CONVERSATION_H
#include <stdlib.h>
typedef enum {
    INVALID,
    OPTIONS,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT,
} method_t;

typedef enum {
    ZERO_NINE,
    ONE_ZERO,
    ONE_ONE,
} version_t;
    
typedef enum {
    CONTINUE = 100,
    SWITCHING_PROTOCOLS = 101,
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NON_AUTHORITATIVE_INFORMATION = 203,
    NO_CONTENT = 204,
    RESET_CONTENT = 205,
    PARTIAL_CONTENT = 206,
    MULTIPLE_CHOICES = 300,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    NOT_MODIFIED = 304,
    USE_PROXY = 305,
    TEMPORARY_REDIRECT = 307,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    PAYMENT_REQUIRED = 402,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    NOT_ACCEPTABLE = 406,
    PROXY_AUTHENTICATION_REQUIRED = 407,
    REQUEST_TIMEOUT = 408, 
    CONFLICT = 409,
    GONE = 410,
    LENGTH_REQUIRED = 411,
    PRECONDITION_FAILED = 412,
    REQUEST_ENTITY_TOO_LARGE = 413,
    REQUEST_URI_TOO_LONG = 414,
    UNSUPPORTED_MEDIA_TYPE = 415,
    REQUESTED_RANGE_NOT_SATISFIABLE = 416,
    EXPECTATION_FAILED = 417,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504,
    HTTP_VERSION_NOT_SUPPORTED = 505,
} status_t;
    
typedef struct key {
    unsigned int src;
    unsigned int dst;
    unsigned short s_port;
    unsigned short d_port;
} hash_key_t;
    
typedef struct tcp_stream {
    char *data;
    int length;
    int reply; /* boolean */
    struct timeval ts;
} tcp_stream_t;

typedef struct header {
    const char *name;
    const char *value;
} header_t;
    
typedef struct message {
    union {
        method_t method;
        status_t status;
    };
    
    version_t version;
    int h_count;
    char *url;        
    union {
        char *query_string;
        char *reason;
    };
    int length;
    char *body; /* NOT NULL terminated */
    header_t *headers;
} message_t;

typedef struct http_conversation {
    tcp_stream_t request;
    tcp_stream_t reply;
    hash_key_t key;
    int fins;

    /* parsed version */
    int r_count;
    message_t *requests;
    message_t *replies;
} http_conversation_t;
#endif /* _HTTP_CONVERSATION_H */


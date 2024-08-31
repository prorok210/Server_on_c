#ifndef PROCESSING_REQ_H
#define PROCESSING_REQ_H

#include <sys/types.h>
#include <libmongoc-1.0/mongoc/mongoc.h>

#define HEADERS_COUNT 50

struct HttpResponse  {
    int status_code;
    char *status;
    char *content_type;
    char *content;
    int content_length;
};

struct Headers {
    char *name;
    char *value;
};

struct HttpRequest {
    char *method;
    char *url;
    char *version;
    struct Headers headers[HEADERS_COUNT];
    char *body;
};

int receive_msg(int client_sock, char* buffer, int buf_size, struct HttpRequest *request);

int send_msg(int client_sock, struct HttpResponse* response);

void format_request(char* request);

struct HttpResponse (*router(char* route))(struct HttpRequest *, mongoc_database_t *);

#endif // PROCESSING_REQ_H
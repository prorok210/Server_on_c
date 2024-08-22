#ifndef PROCESSING_REQ_H
#define PROCESSING_REQ_H

#include <sys/types.h>

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
    struct Headers headers[20];
    char *body;
};

int receive_msg(int client_sock, char* buffer, int buf_size, struct HttpRequest *request);

struct HttpResponse (*router(char* route))(struct HttpRequest *);

#endif // PROCESSING_REQ_H
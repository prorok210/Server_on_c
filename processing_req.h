#ifndef PROCESSING_REQ_H
#define PROCESSING_REQ_H

#include <sys/types.h>

struct HttpResponse  {
    char *status;
    char *content_type;
    char *content;
    int content_length;
};

int receive_msg(int client_sock, char* buffer, int buf_size, struct HttpRequest *request);

int process_request(struct HttpRequest *request, struct HttpResponse *response);

#endif // PROCESSING_REQ_H
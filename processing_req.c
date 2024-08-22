#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "http_pars.h"

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


int receive_msg(int client_sock, char* buffer, int buf_size, struct HttpRequest *request) {
    int bytes_read = read(client_sock, buffer, buf_size);
    if (bytes_read == 0) {
        printf("Client disconnected\n");
        close(client_sock);
        return 0;
    }
    if (bytes_read < 0) {
        perror("Read error");
        return -1;
    }

    printf("Received: %s\n", buffer);
    if (parse_request(buffer, request)!=0) {
        fprintf(stderr, "Error parsing request\n");
        return -1;
    }

    return bytes_read;
}
    
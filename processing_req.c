#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "http_pars.h"


struct HttpResponse  {
    char *status;
    char *content_type;
    char *content;
    int content_length;
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

int process_request(struct HttpRequest *request, struct HttpResponse *response) {
    if (request == NULL || response == NULL) {
        fprintf(stderr, "Invalid pointer to struct\n");
        return 1;
    }

    if (request->method == NULL || request->url == NULL || request->version == NULL) {
        fprintf(stderr, "Invalid request\n");
        return 1;
    }

    if (strcmp(request->method, "GET") == 0) {
        response->status = "HTTP/1.1 200 OK";
        response->content_type = "text/html";
        response->content = "<html><body><h1>Hello, World!</h1></body></html>";
        response->content_length = strlen(response->content);
        return 0;
    }
    else {
        response->status = "HTTP/1.1 405 Method Not Allowed";
        response->content_type = "text/html";
        response->content = "<html><body><h1>Method Not Allowed</hh1></body></html>";
        response->content_length = strlen(response->content);
        return 0;
    }
}
    
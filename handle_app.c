#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "processing_req.h"
#include "server.h"

#define RESPONSE_SIZE 16384


void* app(void* args) {
    struct Args* arg = (struct Args*) args;
    int client_sock = arg->client_sock;
    int buf_size = arg->buf_size;
    char* buffer = arg->buffer;
    struct HttpRequest* rec_request = arg->rec_request;

    int bytes_read = receive_msg(client_sock, buffer, buf_size, rec_request); 
    if (bytes_read < 0) {
        fprintf(stderr, "Error receiving message\n");
        goto cleanup;
    }
    if (bytes_read == 0) {
        fprintf(stderr, "Client disconnected\n");
        goto cleanup;
    }

    struct HttpResponse (*view_function)(struct HttpRequest *);

    view_function = router(rec_request->url);

    struct HttpResponse response = view_function(rec_request);

    char* response_line = (char*)malloc(RESPONSE_SIZE);
    if (response_line == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        goto cleanup;
    }

    int response_len = snprintf(response_line, RESPONSE_SIZE, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s",
                                   response.status_code, response.status, response.content_type, response.content_length, response.content);

    if (response_len < 0 || response_len >= RESPONSE_SIZE) {
        fprintf(stderr, "Error building response\n");
        goto cleanup;
    }

    if (send(client_sock, response_line, strlen(response_line), 0) < 0) {
        perror("Send error");
        goto cleanup;
    }

    cleanup:
        close(client_sock);
        free(rec_request);
        free(buffer);
        free(arg);
        if (response_line)
            free(response_line);
        return NULL;
}



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

    int keep_alive = 1;

    while(keep_alive) {
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
        if (view_function == NULL) {
            fprintf(stderr, "No route matched\n");
            goto cleanup;
        }

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
            free(response_line);
            goto cleanup;
        }

        if (send(client_sock, response_line, strlen(response_line), 0) < 0) {
            fprintf(stderr, "Send error");
            free(response_line);
            goto cleanup;
        }

        keep_alive = 0;
        
        for (int i = 0; i < HEADERS_COUNT; i++) {
            if (rec_request->headers[i].name == NULL) {
                break;
            }
            if ((strcasecmp(rec_request->headers[i].name, "connection")) == 0) {
                if ((strcasecmp(rec_request->headers[i].value, "keep-alive") == 0)) {
                    keep_alive = 1;   
                } else {
                    keep_alive = 0;
                }
                break;
            }
        }

        free(response_line);

        memset(buffer, 0, buf_size);
        free_request(rec_request);
    }
    

    cleanup:
        close(client_sock);
        free(rec_request);
        free(buffer);
        free(arg);
        return NULL;
}



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
            send(client_sock, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n", 45, 0);
            goto cleanup;
        }

        struct HttpResponse response = view_function(rec_request);

        if (send_msg(client_sock, &response) < 0) {
            fprintf(stderr, "Error sending message\n");
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

        memset(buffer, 0, buf_size);
        free_request(rec_request);
    }
    

    cleanup:
        close(client_sock);
        free(rec_request);
        free(buffer);
        free(arg);
        printf("Thread finished, connection above\n");
        return NULL;
}



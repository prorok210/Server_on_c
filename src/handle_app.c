#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <mongoc.h>
#include "../include/processing_req.h"
#include "../include/server.h"
#include "../include/handle_app.h"

// Функция для установки таймаута сокета
void set_socket_timeout(int client_sock, int timeout_sec) {
    struct timeval timeout;
    timeout.tv_sec  = timeout_sec;
    timeout.tv_usec = 0;

    if (setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting receive timeout");
    }

    if (setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting send timeout");
    }
}

// Приложение для обработки запросов, формирование и отправления ответов
void* app(void* args) {
    struct Args* arg = (struct Args*) args;

    int client_sock                        = arg->client_sock;
    int buf_size                           = arg->buf_size;
    char* buffer                           = arg->buffer;
    int keep_alive                         = arg->keep_alive;
    int timeout_s                          = arg->timeout_s;
    int max_requests                       = arg->max_requests;
    int num_of_requests                    = arg->num_of_requests;
    mongoc_database_t* mongoc_database     = arg->db_database;
    struct HttpRequest* rec_request        = arg->rec_request;

    while(keep_alive) {
        if (num_of_requests > max_requests){
            fprintf(stderr, "Too many requests\n");
            goto cleanup;
        }

        //Получение запроса
        int bytes_read = receive_msg(client_sock, buffer, buf_size, rec_request); 
        if (bytes_read < 0) {
            fprintf(stderr, "Error receiving message\n");
            goto cleanup;
        }
        if (bytes_read == 0) {
            fprintf(stderr, "Client disconnected\n");
            goto cleanup;
        }

        keep_alive = 0;
        
        //получение данных из заголовков
        for (int i = 0; i < HEADERS_COUNT; i++) {
            if (rec_request->headers[i].name == NULL) {
                break;
            }

            if ((strcasecmp(rec_request->headers[i].name, "connection")) == 0) {
                if ((strcasecmp(rec_request->headers[i].value, "keep-alive") == 0)) {
                    keep_alive = 1;   
                } else {
                    keep_alive = 0;
                    break;
                }
            }
            if (strcasecmp(rec_request->headers[i].name, "keep-alive") == 0) {
                keep_alive = 1;
                char *timeout_str = strstr(rec_request->headers[i].value, "timeout=");
                if (timeout_str != NULL) {
                    timeout_s = atoi(timeout_str + 8);
                    if ((timeout_s < 0) || (timeout_s > MAX_TIMEOUT)) {
                        timeout_s = TIMEOUT;
                    }
                }
                char *max_requests_str = strstr(rec_request->headers[i].value, "max=");
                if (max_requests_str != NULL) {
                    max_requests = atoi(max_requests_str + 4);
                    if ((max_requests < 0) || (max_requests > MAX_REQUESTS)) {
                        max_requests = MAX_REQUESTS;
                    }
                }
                break;
            }
           
        }

        set_socket_timeout(client_sock, timeout_s);

        struct HttpResponse (*view_function)(struct HttpRequest *, mongoc_database_t *);

        view_function = router(rec_request->url);
        if (view_function == NULL) {
            fprintf(stderr, "No route matched\n");
            send(client_sock, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n", 45, 0);
            goto cleanup;
        }

        struct HttpResponse response = view_function(rec_request, mongoc_database);

        if (send_msg(client_sock, &response) < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                fprintf(stderr, "Send timed out\n");
            } else {
                fprintf(stderr, "Error sending message\n");
            }
            goto cleanup;
        }

        num_of_requests++;

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

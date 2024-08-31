#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/http_pars.h"
#include "../include/processing_req.h"
#include "../include/handle_app.h"
#include "../include/register_views.h"
#include "../include/server.h"
#include "../include/database.h"


int start_server() {
    struct sockaddr_in server_sock_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    socklen_t addrlen = sizeof(server_sock_addr);

    mongoc_client_t *mongoc_client = NULL;
    mongoc_database_t *mongoc_database = NULL;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Socket init error");
        return -1;
    }
    printf("Socket created\n");

    if (bind(sock, (struct sockaddr *)&server_sock_addr, addrlen) != 0) {
        fprintf(stderr, "Bind error");
        return -1;
    }
    printf("Socket bound\n");

    if (connect_to_db("testDB:testDB", "testDB", &mongoc_client, &mongoc_database) != 0) {
        fprintf(stderr, "Connection to database error");
        return -1;
    } else printf("Successes connected to database\n");

    if (check_collections(mongoc_database) != 0) {
        fprintf(stderr, "Checking collections error");
        return -1;
    } else printf("Collections checked\n");

    if (paths()!=0){
        fprintf(stderr, "Loading views error");
        return -1;
    }

    if (listen(sock, BACKLOG) < 0) {
            fprintf(stderr, "Listen error");
            return -1;
        }

    printf("Server was successfully started\n");

    printf("Listening on port %d .....\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        int client = accept(sock, (struct sockaddr *)&client_addr, &client_addrlen);
        if (client < 0) {
            fprintf(stderr, "Accept error");
            continue;
        }

        printf("Client connected\n");

        struct Args *arg = malloc(sizeof(struct Args));
        if (arg == NULL){
            fprintf(stderr, "Memory allocation error");
            return -1;
        }
        arg->client_sock     = client;
        arg->buffer          = malloc(BUF_SIZE);
        arg->buf_size        = BUF_SIZE;
        arg->keep_alive      = 1;      
        arg->timeout_s       = TIMEOUT; 
        arg->max_requests    = MAX_REQUESTS;
        arg->num_of_requests = 0;
        arg->db_client       = mongoc_client;
        arg->db_database     = mongoc_database;
        arg->rec_request     = malloc(sizeof(struct HttpRequest));
        if (arg->rec_request == NULL) {
            fprintf(stderr, "Memory allocation error");
            return -1;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, app, (void *)arg) != 0) {
            fprintf(stderr, "Thread creation error\n");
            close(client);
            free(arg->rec_request);
            free(arg->buffer);
            free(arg);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(sock);
    return 0;
}

int main() {
    start_server();
    return 0;
}

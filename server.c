#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "http_pars.h"

#define PORT 8080
#define BUF_SIZE 1024
#define BACKLOG 10

// int socket(int domain, int type, int protocol); //(AF_INET, AF_INET6) Возвращает дискриптор сокета
// int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
// int listen(int sockfd, int backlog); int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// int read(); int recv(); int recvfrom(); int recvmsg();
// int write(); int send(); int sendto(); int sendmsg();
// int close();shutdown(); 


char buffer[BUF_SIZE] = {0};

struct Args {
    int client_sock;
    char *buffer;
    int buf_size;
    struct HttpRequest *rec_request;
};

int start_server() {
    struct sockaddr_in server_sock_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    socklen_t addrlen = sizeof(server_sock_addr);


    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Socket init error");
        return 1;
    }
    printf("Socket created\n");

    if (bind(sock, (struct sockaddr *)&server_sock_addr, addrlen) != 0) {
        fprintf(stderr, "Bind error");
        return 1;
    }
    printf("Socket bound\n");

    if (listen(sock, BACKLOG) < 0) {
            fprintf(stderr, "Listen error");
            return 1;
        }

    printf("Listening on port %d .....\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addrlen = sizeof(client_addr);

        int client = accept(sock, (struct sockaddr *)&client_addr, &client_addrlen);
        if (client < 0) {
            fprintf(stderr, "Accept error");
            return 1;
        }

        printf("Client connected\n");

        pthread_t thread;

        struct Args *arg = malloc(sizeof(struct Args));
        arg->client_sock = client;
        arg->buffer = buffer;
        arg->buf_size = BUF_SIZE;
        arg->rec_request = malloc(sizeof(struct HttpRequest));
        if (arg->rec_request == NULL) {
            fprintf(stderr, "Memory allocation error");
            return 1;
        }
        if (pthread_create(&thread, NULL, receive_msg, arg)<0) {
            fprintf(stderr, "Thread create error");
            return 1;
        }
        pthread_join(thread, NULL);
    }

    close(sock);
    return 0;
}



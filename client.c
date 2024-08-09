#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 1024

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        fprintf(stderr, "Socket creation error");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Connect error");
        close(sock);
        return 1;
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    char buffer[BUF_SIZE] = "Hello, server!";
    if (send(sock, buffer, strlen(buffer), 0) < 0) {
        fprintf("Send error");
        close(sock);
        return 1;
    }

    printf("Message sent: %s\n", buffer);

    memset(buffer, 0, BUF_SIZE);  // Clear buffer
    int bytes_received = recv(sock, buffer, BUF_SIZE, 0);
    if (bytes_received < 0) {
        fprintf(stderr, "Receive error");
        close(sock);
        return 1;
    } else if (bytes_received == 0) {
        printf("Server closed the connection\n");
    } else {
        printf("Received from server: %s\n", buffer);
    }

    close(sock);
    return 0;
}

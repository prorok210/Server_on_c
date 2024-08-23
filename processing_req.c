#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "http_pars.h"
#include "register_views.h"
#include "processing_req.h"
#include "handle_app.h"


int receive_msg(int client_sock, char* buffer, int buf_size, struct HttpRequest *request) {
    int bytes_read = read(client_sock, buffer, buf_size);
    if (bytes_read == 0) {
        printf("Client disconnected\n");
        close(client_sock);
        return 0;
    }
    if (bytes_read < 0) {
        fprintf(stderr, "Error reading from socket\n");
        return -1;
    }

    format_request(buffer);

    if (parse_request(buffer, request)!=0) {
        fprintf(stderr, "Error parsing request\n");
        return -1;
    }

    return bytes_read;
}


int send_msg(int client_sock, struct HttpResponse* response) {
    char* response_line = (char*)malloc(RESPONSE_SIZE);
        if (response_line == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            return -1;
        }
    int response_len = snprintf(response_line, RESPONSE_SIZE, "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n%s",
                                response->status_code, response->status, response->content_type, response->content_length, response->content);
        
    if (response_len < 0 || response_len >= RESPONSE_SIZE) {
            fprintf(stderr, "Error building response\n");
            free(response_line);
            return -1;
        }

    if (send(client_sock, response_line, strlen(response_line), 0) < 0) {
        fprintf(stderr, "Send error");
        free(response_line);
        return -1;
    }

    free(response_line);
    return 0;
}


void format_request(char* request) {
    while(*request != '\0') {
        if (*request == '"')
            *request = '\'';
        request++;
    }
}


struct HttpResponse (*router(char* route))(struct HttpRequest *) {
    struct Views *current = header_view;
    while (current != NULL) {
        if (strcmp(current->route, route) == 0) {
            return current->view;
        }
        current = current->next;
    }
    return NULL;
}
    
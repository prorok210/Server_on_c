#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "http_pars.h"
#include "register_views.h"
#include "processing_req.h"


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
    
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "http_pars.h"


int app(int client_sock, char *buffer, int buf_size, struct HttpRequest *rec_request) {
    int bytes_read = receive_msg(client_sock, buffer, buf_size, rec_request); 
    if (bytes_read < 0) {
        fprintf(stderr, "Error receiving message\n");
        return 1;
    }
}



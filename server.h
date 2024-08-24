#ifndef SERVER_H
#define SERVER_H

#include "http_pars.h"   // Для структуры HttpRequest

#define PORT 8080
#define BUF_SIZE 16384
#define BACKLOG 10

// Структура для передачи аргументов в поток
struct Args {
    int client_sock;
    char *buffer;
    int buf_size;
    int keep_alive;
    int timeout_s;
    int max_requests;
    int num_of_requests;
    struct HttpRequest *rec_request;
};

// Функция запуска сервера
int start_server(void);

#endif // SERVER_H
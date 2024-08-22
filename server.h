#ifndef SERVER_H
#define SERVER_H

#include "http_pars.h"   // Для структуры HttpRequest

#define PORT 8080
#define BUF_SIZE 16384
#define BACKLOG 10

// Структура для передачи аргументов в поток
struct Args {
    int client_sock;            // Дескриптор сокета клиента
    char *buffer;               // Буфер для получения данных
    int buf_size;               // Размер буфера
    struct HttpRequest *rec_request;  // Структура для хранения распарсенного HTTP-запроса
};

// Функция запуска сервера
int start_server(void);

#endif // SERVER_H
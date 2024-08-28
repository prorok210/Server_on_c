#ifndef SERVER_H
#define SERVER_H

#include "../include/http_pars.h"   // Для структуры HttpRequest
#include <mongoc/mongoc.h>          // Для работы с MongoDB

#define PORT 8080
#define BUF_SIZE 16384
#define BACKLOG 10

// Структура для передачи аргументов в поток
struct Args {
    int client_sock;                     // Сокет клиента
    char *buffer;                        // Буфер для чтения запроса
    int buf_size;                        // Размер буфера
    int keep_alive;                      // Флаг keep-alive
    int timeout_s;                       // Время таймаута
    int max_requests;                    // Максимальное количество запросов
    int num_of_requests;                 // Счетчик запросов
    mongoc_client_t *db_client;      // Клиент MongoDB(другой бд)
    mongoc_database_t *db_database;  // Струтура для бд
    struct HttpRequest *rec_request;     // Структура для обработки запроса
};

// Функция запуска сервера
int start_server(void);

#endif // SERVER_H
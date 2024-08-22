#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <sys/types.h>

#define HEADERS_COUNT 20

// Функция для парсинга запроса
int parse_request(const char *request, struct HttpRequest *Req);

// Функция для освобождения памяти, выделенной под запрос
void free_request(struct HttpRequest *Req);

#endif // HTTP_PARSER_H
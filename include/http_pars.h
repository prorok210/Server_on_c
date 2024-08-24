#ifndef HTTP_PARS_H
#define HTTP_PARS_H

#include <sys/types.h>
#include "../include/processing_req.h"

#define HEADERS_COUNT 50

// Функция для парсинга запроса
int parse_request(const char *request, struct HttpRequest *Req);

// Функция для освобождения памяти, выделенной под запрос
void free_request(struct HttpRequest *Req);

#endif // HTTP_PARS_H
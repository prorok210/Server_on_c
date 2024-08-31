#ifndef REGISTER_VIEWS_H
#define REGISTER_VIEWS_H

#include "../include/processing_req.h"
#include <libmongoc-1.0/mongoc/mongoc.h>

//Прототип функции-обработчика запроса
struct HttpResponse view_func(struct HttpRequest *request, mongoc_database_t *database);

//Структура для хранения роутов и указателей на функции-обработчики
struct Views {
    char *route;
    struct HttpResponse (*view)(struct HttpRequest *request, mongoc_database_t *database);
    struct Views *next;
};

extern struct Views *header_view;

int register_view(char *route, struct HttpResponse (*view)(struct HttpRequest *request, mongoc_database_t *database));

int paths();

#endif//REGISTER_VIEWS_H
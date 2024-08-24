#ifndef REGISTER_VIEWS_H
#define REGISTER_VIEWS_H

#include "../include/processing_req.h"

struct HttpResponse view_func(struct HttpRequest *request);

struct Views {
    char *route;
    struct HttpResponse (*view)(struct HttpRequest *request);
    struct Views *next;
};

extern struct Views *header_view;

int register_view(char *route, struct HttpResponse (*view)(struct HttpRequest *request));

int paths();

#endif//REGISTER_VIEWS_H
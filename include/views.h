#ifndef VIEWS_H
#define VIEWS_H

#include "../include/processing_req.h"
#include <libmongoc-1.0/mongoc/mongoc.h>

struct HttpResponse test_func(struct HttpRequest *request, mongoc_database_t *database);
struct HttpResponse create_user(struct HttpRequest *request, mongoc_database_t *database);
struct HttpResponse get_users(struct HttpRequest *request, mongoc_database_t *database);

#endif // VIEWS_H
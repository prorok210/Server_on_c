#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/processing_req.h"

struct HttpResponse view_func(struct HttpRequest *request, mongoc_database_t *database);

struct Views {
    char *route;
    struct HttpResponse (*view)(struct HttpRequest *request, mongoc_database_t *database);
    struct Views *next;
};

struct Views *header_view = NULL;

int register_view(char *route, struct HttpResponse (*view)(struct HttpRequest *request, mongoc_database_t *database)) {
    struct Views *new_view = (struct Views *)malloc(sizeof(struct Views));
    if (new_view == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return -1;
    }

    new_view->route = strdup(route);
    if (new_view->route == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        free(new_view);
        return -1;
    }

    new_view->view = view;
    new_view->next = NULL;

    if (header_view == NULL) {
        header_view = new_view;
    } else {
        struct Views *current = header_view;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_view;
    }

    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../include/views.h"


//функция для регистрации роутов. Они будут включены в основную программу

int paths() {
    if (register_view("/test", test_func)!=0) {
        return -1;
    }
    if (register_view("/create_user", create_user)!=0) {
        return -1;
    }
    if (register_view("/get_users", get_users)!=0) {
        return -1;
    }
    return 0;
}
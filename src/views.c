#include "../include/processing_req.h"
#include <libmongoc-1.0/mongoc/mongoc.h>
#include <bson/bson.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

// Bad Request
const struct HttpResponse bad_request = { 
    .status_code = 400,
    .status = "Bad Request",
    .content_type = "text/html",
    .content = "<html><body><h1>Bad Request</h1></body></html>",
    .content_length = strlen("<html><body><h1>Bad Request</h1></body></html>")
};
// Not Found
const struct HttpResponse not_found = { 
    .status_code = 404,
    .status = "Not Found",
    .content_type = "text/html",
    .content = "<html><body><h1>Not Found</h1></body></html>",
    .content_length = strlen("<html><body><h1>Not Found</h1></body></html>")
};
// Method Not Allowed
const struct HttpResponse method_not_allowed = { 
    .status_code = 405,
    .status = "Method Not Allowed",
    .content_type = "text/html",
    .content = "<html><body><h1>Method Not Allowed</h1></body></html>",
    .content_length = strlen("<html><body><h1>Method Not Allowed</h1></body></html>")
};
// Internal Server Error
const struct HttpResponse internal_server_error = {
    .status_code = 500,
    .status = "Internal Server Error",
    .content_type = "text/html",
    .content = "<html><body><h1>Internal Server Error</h1></body></html>",
    .content_length = strlen("<html><body><h1>Internal Server Error</h1></body></html>")
};

// прототип функции-обработчика struct HttpResponse view_func(struct HttpRequest *request, mongoc_database_t *database);


struct HttpResponse test_func(struct HttpRequest *request, mongoc_database_t *database) {
    struct HttpResponse response;
    response.status_code = 200;
    response.status = "OK";
    response.content_type = "text/html";
    response.content = "<html><body><h1>Test page</h1></body></html>";
    response.content_length = strlen(response.content);
    return response;
}


struct HttpResponse create_user(struct HttpRequest *request, mongoc_database_t *database) {
    if (strcmp(request->method, "POST") != 0) {
        return method_not_allowed;
    }
    char* body_str = strdup(request->body);
    if (body_str == NULL) {
        fprintf(stderr, "Body is empty\n");
        return bad_request;
    }
    for (size_t i = 0; i < strlen(body_str); i++) {
        if (body_str[i] == '\'') {
            body_str[i] = '\"';
        }
    }

    cJSON *body_json = cJSON_Parse(body_str);
    free(body_str);
    if (body_json == NULL) {
        fprintf(stderr, "Error parsing JSON\n");
        return bad_request;
    }

    cJSON *name = cJSON_GetObjectItem(body_json, "name");
    cJSON *age = cJSON_GetObjectItem(body_json, "age");

    if (!(cJSON_IsString(name) && (name->valuestring != NULL))) {   
        fprintf(stderr, "Name is not a valid string.\n");
        cJSON_Delete(body_json);
        return internal_server_error;
    }

    if (!(cJSON_IsNumber(age))) { 
        fprintf(stderr, "Age is not a valid number.\n");
        cJSON_Delete(body_json);
        return internal_server_error;
    }

    bson_t *doc = bson_new();
    bson_error_t error = {0};
    BSON_APPEND_UTF8(doc, "name", name->valuestring);
    BSON_APPEND_INT32(doc, "age", age->valueint);

    if (!mongoc_collection_insert_one(mongoc_database_get_collection(database, "users"), doc, NULL, NULL, &error)) {
        fprintf(stderr, "Error inserting document: %s\n", error.message);
        bson_destroy(doc);
        cJSON_Delete(body_json);
        return internal_server_error;
    }

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(mongoc_database_get_collection(database, "users"), doc, NULL, NULL);
    const bson_t *found_doc;

    if (mongoc_cursor_next(cursor, &found_doc)) {
        char *json_str = bson_as_canonical_extended_json(found_doc, NULL);

        struct HttpResponse response = {
            .status_code = 201,
            .status = "Created",
            .content_type = "application/json",
            .content = json_str,
            .content_length = strlen(json_str)
        };

        mongoc_cursor_destroy(cursor);
        bson_destroy(doc);
        cJSON_Delete(body_json);
        return response;
    } else {
        mongoc_cursor_error_document(cursor, &error, &found_doc);
        fprintf(stderr, "Error fetching inserted document: %s\n", error.message);
        mongoc_cursor_destroy(cursor);
        bson_destroy(doc);
        cJSON_Delete(body_json);
        return internal_server_error;
    }
}

struct HttpResponse get_users(struct HttpRequest *request, mongoc_database_t *database) {
    if (strcmp(request->method, "GET") != 0) {
        return method_not_allowed;
    }

    bson_t *query = bson_new();

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(mongoc_database_get_collection(database, "users"), query, NULL, NULL);
    const bson_t *doc;
    char *json_str = strdup("[");
    if (json_str == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return internal_server_error;
    }

    while (mongoc_cursor_next(cursor, &doc)) {
        char *doc_str = bson_as_canonical_extended_json(doc, NULL);
        if (doc_str == NULL) {
            fprintf(stderr, "Error converting BSON to JSON\n");
            free(json_str);
            mongoc_cursor_destroy(cursor);
            return internal_server_error;
        }
        json_str = realloc(json_str, strlen(json_str) + strlen(doc_str) + 2);
        if (json_str == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            free(doc_str);
            mongoc_cursor_destroy(cursor);
            return internal_server_error;
        }
        strcat(json_str, doc_str);
        strcat(json_str, ",");
        free(doc_str);
    }

    json_str[strlen(json_str) - 1] = ']';

    struct HttpResponse response = {
        .status_code = 200,
        .status = "OK",
        .content_type = "application/json",
        .content = json_str,
        .content_length = strlen(json_str)
    };

    mongoc_cursor_destroy(cursor);
    bson_destroy(query);
    return response;
}
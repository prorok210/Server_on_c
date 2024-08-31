#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/http_pars.h"
#include "../include/processing_req.h"


int parse_request(const char *request, struct HttpRequest *Req) {
    if (Req == NULL) {
        fprintf(stderr, "Invalid pointer to struct in file %s at line %d\n", __FILE__, __LINE__);
        return 1;
    }

    struct HttpRequest *temp_ptr = malloc(sizeof(struct HttpRequest));
    if (temp_ptr == NULL) {
        fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
        return 1;
    }
    memset(temp_ptr, 0, sizeof(struct HttpRequest));

    char *req_copy = (char *)malloc(strlen(request) + 1);
    if (req_copy == NULL) {
        fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
        free(temp_ptr);
        return 1;
    }

    strcpy(req_copy, request);

    if (req_copy == NULL || req_copy[0] == '\0' || req_copy[0] == '\n' || req_copy[0] == '\r' || req_copy[0] == ' ' || req_copy[0] == '\t' || (strstr(req_copy, "\r\n") == NULL)) {
        fprintf(stderr, "Request is empty in file %s at line %d\n", __FILE__, __LINE__);
        free(req_copy);
        free(temp_ptr);
        return 1;
    }

    char* saveptr;

    char* end_of_headers = strstr(req_copy, "\r\n\r\n");
    if (end_of_headers != NULL) {
        end_of_headers += 4;
    } else {
        fprintf(stderr, "Invalid request in file %s at line %d\n", __FILE__, __LINE__);
        free_request(temp_ptr);
        free(req_copy);
        return 1;
    }

    char* body = malloc(strlen(end_of_headers) + 1);
    if (body == NULL) {
        fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
        free_request(temp_ptr);
        free(req_copy);
        return 1;
    }
    strcpy(body, end_of_headers);

  
    char* line = strtok_r(req_copy, "\r\n", &saveptr);
    if (line == NULL){
        fprintf(stderr, "Request is empty in file %s at line %d\n", __FILE__, __LINE__);
        free(req_copy);
        free(temp_ptr);
        return 1;
    }
    
    // основная информация из запроса
    char *method = strtok(line, " ");
    if (method) {
        temp_ptr->method = strdup(method);
        if (temp_ptr->method == NULL) {
            fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
            free(req_copy);
            free (temp_ptr);
            return 1;
        }
    }

    char *url = strtok(NULL, " ");
    if (url) {
        temp_ptr->url = strdup(url);
        if (temp_ptr->url == NULL) {
            fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
            free(req_copy);
            free(temp_ptr->method);
            free(temp_ptr);
            return 1;
        }
    }

    char *version = strtok(NULL, "\r\n");
    if (version) {
        temp_ptr->version = strdup(version);
        if (temp_ptr->version == NULL) {
            fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
            free(temp_ptr->method);
            free(temp_ptr->url);
            free(temp_ptr);
            free(req_copy);
            return 1;
        }
    }
    if (temp_ptr->method == NULL || temp_ptr->url == NULL || temp_ptr->version == NULL) {
        fprintf(stderr, "Invalid request in file %s at line %d\n", __FILE__, __LINE__);
        free_request(temp_ptr);
        free(req_copy);
        return 1;
    }

    // заголовки запроса
    int header_count = 0;

    while ((line = strtok_r(saveptr, "\r\n", &saveptr))!= NULL) {
        if ((line == end_of_headers) ||(line[0] == '\0'))
            break;

        if (header_count >= HEADERS_COUNT) {
            fprintf(stderr, "Too many headers in file %s at line %d\n", __FILE__, __LINE__);
            free(req_copy);
            free_request(temp_ptr);
            return 1;
        }

        char *name = strtok(line, ":");
        if (name){
            temp_ptr->headers[header_count].name = strdup(name);
            if (temp_ptr->headers[header_count].name == NULL) {
                fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
                free(req_copy);
                free_request(temp_ptr);
                return 1;
            }
        }
            
        char *header_value = strtok(NULL, "\r\n");
        if (header_value) {
            while (*header_value== ' ') {
                header_value++;
            }
            temp_ptr->headers[header_count].value = strdup(header_value);
            if (temp_ptr->headers[header_count].value == NULL) {
                fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
                free(req_copy);
                free_request(temp_ptr);
                return 1;
            }
        }

        header_count++;
    }

    //тело запроса
    if (body) {
        temp_ptr->body = strdup(body);
        if (temp_ptr->body == NULL) {
            fprintf(stderr, "Memory allocation error in file %s at line %d\n", __FILE__, __LINE__);
            free(req_copy);
            free_request(temp_ptr);
            return 1;
        }
    }

    *Req = *temp_ptr;

    if (req_copy) free(req_copy);
    return 0;
}


void free_request(struct HttpRequest *Req) {
    if (Req == NULL) return;

    // Освобождение динамически выделенной памяти для метода, URL и версии
    if (Req->method) free(Req->method);
    if (Req->url) free(Req->url);
    if (Req->version) free(Req->version);

    // Освобождение динамически выделенной памяти для значений заголовков
    for (int i = 0; i < HEADERS_COUNT; i++) {
        if (Req->headers[i].name == NULL) {
            break;
        }
        free(Req->headers[i].name);
        if (Req->headers[i].value) free(Req->headers[i].value);
    }

    // Освобождение тела запроса
    if (Req->body) free(Req->body);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define HEADERS_COUNT 20

//Парсес запроса
struct Headers {
    char *name;
    char *value;
};

struct HttpRequest {
    char *method;
    char *url;
    char *version;
    struct Headers headers[HEADERS_COUNT];
    char *body;
};


int parse_request(const char *request, struct HttpRequest *Req) {
    if (Req == NULL) {
        fprintf(stderr, "Invalid pointer to struct", __FILE__, __LINE__);
        return 1;
    }

    struct HttpRequest *temp_ptr = malloc(sizeof(struct HttpRequest));
    if (temp_ptr == NULL) {
        fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
        return 1;
    }
    memset(temp_ptr, 0, sizeof(struct HttpRequest));

    char *req_copy = malloc(strlen(request) + 1);
    if (req_copy == NULL) {
        fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
        return 1;
    }
    if (strcpy(req_copy, request) < 0) {
        fprintf(stderr, "Copy error", __FILE__, __LINE__);
        return 1;
    }
    for (int i = 0; i < strlen(req_copy); i++) {
        printf("%c", req_copy[i]);
    }

    printf("Request: %s\n", req_copy);

    if (req_copy == NULL || req_copy[0] == '\0' || req_copy[0] == '\n' || req_copy[0] == '\r' || req_copy[0] == ' ' || req_copy[0] == '\t' || (strstr(req_copy, "\r\n") == NULL)) {
        fprintf(stderr, "Request is empty", __FILE__, __LINE__);
        return 1;
    }
    
    printf("Request: %s\n", req_copy);



    char* saveptr;
  
    char* line = strtok_r(req_copy, "\r\n", &saveptr);
    if (line == NULL){
        fprintf(stderr, "Request is empty", __FILE__, __LINE__);
        free(req_copy);
        return 1;
    }
        

    char *method = strtok(line, " ");
    if (method) {
        temp_ptr->method = strdup(method);
        if (temp_ptr->method == NULL) {
            fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
            free(req_copy);
            return 1;
        }
    }

    char *url = strtok(NULL, " ");
    if (url) {
        temp_ptr->url = strdup(url);
        if (temp_ptr->url == NULL) {
            fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
            free(req_copy);
            return 1;
        }
    }

    char *version = strtok(NULL, "\r\n");
    if (version) {
        temp_ptr->version = strdup(version);
        if (temp_ptr->version == NULL) {
            fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
            free(req_copy);
            return 1;
        }
    }

    int header_count = 0;

    while ((line = strtok_r(saveptr, "\r\n", &saveptr)) != NULL) {
        if (saveptr[0] == '\n' && saveptr[1] == '\r') {
            break;
        }
        if (header_count >= HEADERS_COUNT) {
            fprintf(stderr, "Too many headers", __FILE__, __LINE__);
            free(req_copy);
            return 1;
        }

        char *name = strtok(line, ":");
        if (name){
            temp_ptr->headers[header_count].name = strdup(name);
            if (temp_ptr->headers[header_count].name == NULL) {
                fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
                free(req_copy);
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
                fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
                free(req_copy);
                return 1;
            }
        }

        header_count++;    
    }

    char *body = strtok(NULL, "\r\n");
    if (body) {
        temp_ptr->body = strdup(body);
        if (temp_ptr->body == NULL) {
            fprintf(stderr, "Memory allocation error", __FILE__, __LINE__);
            free(req_copy);
            return 1;
        }
    }
    Req = temp_ptr;
    
    free(temp_ptr);
    free(req_copy);
    return 0;
}






void free_request(struct HttpRequest *Req) {
    if (Req == NULL) return;

    // Освобождение динамически выделенной памяти для метода, URL и версии
    free(Req->method);
    free(Req->url);
    free(Req->version);

    // Освобождение динамически выделенной памяти для значений заголовков
    for (int i = 0; i < HEADERS_COUNT; i++) {
        if (Req->headers[i].name == NULL) {
            break;
        }
        free(Req->headers[i].name);
        free(Req->headers[i].value);
    }

    // Освобождение тела запроса
    free(Req->body);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_pars.h"
#include "register_views.h"


// Примеры запросов для тестирования
const char* test_req[] = {
    "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: TestAgent\r\nContent-Length: 13\r\n\r\nHello, World!\r\n",

    "GET /index.html HTTP/1.1\r\n",

    ""
};


// Функция для тестирования парсера
int parser_test() {
    int success_case1 = 1, success_case2 = 1, success_case3 = 1;
    printf("Starting parser tests\n");

    struct HttpRequest *case1 = malloc(sizeof(struct HttpRequest));
    struct HttpRequest *case2 = malloc(sizeof(struct HttpRequest));
    struct HttpRequest *case3 = malloc(sizeof(struct HttpRequest));

    printf("Case 1\n");
    parse_request(test_req[0], case1);
    {
        if (
            case1->method != NULL && strcmp(case1->method, "GET") == 0 &&
            case1->url != NULL && strcmp(case1->url, "/index.html") == 0 &&
            case1->version != NULL && strcmp(case1->version, "HTTP/1.1") == 0 &&
            case1->headers[0].name != NULL && strcmp(case1->headers[0].name, "Host") == 0 &&
            case1->headers[0].value != NULL && strcmp(case1->headers[0].value, "www.example.com") == 0 &&
            case1->headers[1].name != NULL && strcmp(case1->headers[1].name, "User-Agent") == 0 &&
            case1->headers[1].value != NULL && strcmp(case1->headers[1].value, "TestAgent") == 0 &&
            case1->headers[2].name != NULL && strcmp(case1->headers[2].name, "Content-Length") == 0 &&
            case1->headers[2].value != NULL && strcmp(case1->headers[2].value, "13") == 0 &&
            case1->body != NULL && strcmp(case1->body, "Hello, World!") == 0
        ) {
            printf("Case 1 passed\n");
        } else {
            success_case1 = 0;
            printf("Case 1 failed\n");
        }
        free_request(case1);
    }



    printf("Case 2\n");
    parse_request(test_req[1], case2);
    {
        if (
            case2->method != NULL && strcmp(case2->method, "GET") == 0 &&
            case2->url != NULL && strcmp(case2->url, "/index.html") == 0 &&
            case2->version != NULL && strcmp(case2->version, "HTTP/1.1") == 0 &&
            case2->headers[0].name == NULL &&
            case2->headers[0].value == NULL &&
            case2->body == NULL
        ) {
            printf("Case 2 passed\n");
        } else {
            success_case2 = 0;
            printf("Case 2 failed\n");
        }
        free_request(case2);
    }

    printf("Case 3\n");
    if (parse_request(test_req[2], case3) == 1) {
        printf("Case 3 passed\n");
    } else {
            success_case3 = 0;
            printf("Case 3 failed\n");
        }
        free_request(case3);

    if (success_case1 && success_case2 && success_case3) {
        return 0;
    } else {
        return 1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// функция для тестирования регистрации функций обработки запросов
struct HttpResponse test_view_func(struct HttpRequest *request) {
        struct HttpResponse response;
        response.status_code = 200;
        response.status= "OK";
        response.content_type = "text/plain";
        response.content = "Test successful!";
        response.content_length = strlen(response.content);
        return response;
    }


int register_test() {
    printf("Starting register views tests\n");
    int success_case1 = 1, success_case2 = 1, success_case3 = 1;
    if (register_view("/test/1", test_view_func) == 0) {
        printf("Case 1 passed\n");
    } else {
        success_case1 = 0;
        printf("Case 1 failed\n");
    }
    if (register_view("/test/2", test_view_func) == 0) {
        printf("Case 2 passed\n");
    } else {
        success_case2 = 0;
        printf("Case 2 failed\n");
    }
    if (register_view("/test/3", test_view_func) == 0) {
        printf("Case 3 passed\n");
    } else {
        success_case3 = 0;
        printf("Case 3 failed\n");
    }
    
    printf("%s\n", header_view->route);
    printf("%s\n", header_view->next->route);
    printf("%s\n", header_view->next->next->route);

    if (success_case1 && success_case2 && success_case3) {
        return 0;
    } else {
        return 1;
    }
}








int main() {
    if (parser_test() == 0) {
        printf("All parser tests passed\n");
    } else {
        printf("Some parser tests failed\n");
    }

    if (register_test() == 0) {
        printf("All register views tests passed\n");
    } else {
        printf("Some register views tests failed\n");
    }
    return 0;
}
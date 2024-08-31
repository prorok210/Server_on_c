#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/http_pars.h"
#include "../include/register_views.h"
#include "../include/database.h"


// Примеры запросов для тестирования
const char* test_req[] = {
    "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: TestAgent\r\nContent-Length: 13\r\n\r\nHello, World!\r\n",

    "GET /index.html HTTP/1.1\r\n\r\n",

    "",

    "GET /test HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: keep-alive\r\n"
    "Cache-Control: max-age=0\r\nsec-ch-ua: 'Chromium';v='128', 'Not;A=Brand';v='24', 'Google Chrome';v='128'\r\n"
    "sec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: 'Windows'\r\nUpgrade-Insecure-Requests: 1\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
    "Sec-Fetch-Site: cross-site\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\n"
    "Accept-Encoding: gzip, deflate, br, zstd\r\nAccept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\r\nCookie: csrftoken=PbVuRQkvxXtyoyBLSkuCMhrpJI4c34WY\r\n\r\n"
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
        };

    printf("Case 4\n");

    struct HttpRequest *case4 = malloc(sizeof(struct HttpRequest));
    parse_request(test_req[3], case4);
    printf("Method: %s\n", case4->method);
    printf("URL: %s\n", case4->url);
    printf("Version: %s\n", case4->version);
    printf("Headers:\n");
    for (int i = 0; i < HEADERS_COUNT; i++) {
        if (case4->headers[i].name == NULL) {
            break;
        }
        printf("%s: %s\n", case4->headers[i].name, case4->headers[i].value);
    }

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

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//тестирование подключение к базе данных
int db_test(){
    printf("DB test\n");
    const char* connection_details = "testDB:testDB"; // "<user>:<password>"
    const char* database_name = "testDB";
    mongoc_client_t *client; 
    mongoc_database_t *database;
    if (connect_to_db(connection_details, database_name, &client, &database) != 0) {
        return 1;
    }
    mongoc_database_destroy(database);
    mongoc_client_destroy(client);
    mongoc_cleanup();
    return 0;
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

    if (db_test() == 0) {
        printf("DB test passed\n");
    } else {
        printf("DB test failed\n");
    }

    return 0;
}
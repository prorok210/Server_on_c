#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_pars.h"

// Примеры запросов для тестирования
const char* test_req[] = {
    "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: TestAgent\r\nContent-Length: 13\r\n\r\nHello, World!\r\n",

    "GET /index.html HTTP/1.1\r\n",

    "",

    "GET / HTTP/1.1\r\n"
    "Host: 127.0.0.1:8080\r\n"
    "Connection: keep-alive\r\n"
    "sec-ch-ua: 'Not)A;Brand';v='99', 'Google Chrome';v='127', 'Chromium';v='127' \r\n"
    "sec-ch-ua-mobile: ?0 \r\n"
    "sec-ch-ua-platform: 'Windows' \r\n"
    "Upgrade-Insecure-Requests: 1 \r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36 \r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7 \r\n"
    "Sec-Fetch-Site: none\r\n"
    "Sec-Fetch-Mode: navigate\r\n"
    "Sec-Fetch-User: ?1\r\n"
    "Sec-Fetch-Dest: document\r\n"
    "Accept-Encoding: gzip, deflate, br, zstd\r\n"
    "Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
    "Cookie: csrftoken=PbVuRQkvxXtyoyBLSkuCMhrpJI4c34WY"
};



// Функция для тестирования парсера
int parser_test() {
    int success_case1 = 1, success_case2 = 1, success_case3 = 1;
    printf("Starting tests\n");

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

    printf("Case 4\n");
    struct HttpRequest *case4 = malloc(sizeof(struct HttpRequest));
    parse_request(test_req[3], case4);
    printf("%s\n", case4->method);
    printf("%s\n", case4->url);
    printf("%s\n", case4->version);
    printf("%s\n", case4->headers[0].name);
    printf("%s\n", case4->headers[0].value);
    printf("%s\n", case4->headers[1].name);
    printf("%s\n", case4->headers[1].value);
    printf("%s\n", case4->headers[2].name);
    printf("%s\n", case4->headers[2].value);
    printf("%s\n", case4->headers[10].name);
    printf("%s\n", case4->headers[10].value);

    if (success_case1 && success_case2 && success_case3) {
        return 0;
    } else {
        return 1;
    }
}

int main() {
    if (parser_test() == 0) {
        printf("All tests passed\n");
    } else {
        printf("Some tests failed\n");
    }
    return 0;
}
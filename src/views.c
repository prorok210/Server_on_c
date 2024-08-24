#include "../include/processing_req.h"
#include <string.h>

// прототип функции-обработчика struct HttpResponse view_func(struct HttpRequest *request);


struct HttpResponse test_func(struct HttpRequest *request) {
    struct HttpResponse response;
    response.status_code = 200;
    response.status = "OK";
    response.content_type = "text/html";
    response.content = "<html><body><h1>Test page</h1></body></html>";
    response.content_length = strlen(response.content);
    return response;
}
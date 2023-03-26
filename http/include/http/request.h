#ifndef HTTPSERVER_REQUEST_H
#define HTTPSERVER_REQUEST_H

#define MAX_REQUEST_URI_LENGTH 8192

#include <core-lib/objects.h>

enum http_method {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_HEAD,
};

struct http_request {
    enum http_method method;
    char request_uri[MAX_REQUEST_URI_LENGTH];
};

enum read_request_result {
    READ_REQUEST_SUCCESS,
    READ_REQUEST_ERROR,
    READ_REQUEST_EOF,
};

// returns read_request_result
int read_request(int fd, struct state_object * so, struct http_request * req);

#endif //HTTPSERVER_REQUEST_H

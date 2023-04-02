#ifndef HTTPSERVER_REQUEST_H
#define HTTPSERVER_REQUEST_H

#define MAX_REQUEST_URI_LENGTH 8192

#include <core-lib/objects.h>

/**
 * Provide the type of request: GET, POST, or HEAD
 */
enum http_method {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_HEAD,
};

/**
 * By the documentation, we are supposed to provide a method and uri that is array of chars
 */
struct http_request {
    enum http_method method;
    char request_uri[MAX_REQUEST_URI_LENGTH];
};

/**
 * Provide the code of success
 */
enum read_request_result {
    READ_REQUEST_SUCCESS,
    READ_REQUEST_INTERNAL_ERROR,
    READ_REQUEST_EOF,
    READ_REQUEST_BAD_REQUEST,
};

/**
 * returns read_request_result
 * @param fd
 * @param so
 * @param req
 * @return
 */
enum read_request_result read_request(int fd, struct state_object * so, struct http_request * req);

#endif //HTTPSERVER_REQUEST_H

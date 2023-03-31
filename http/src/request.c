#include "request.h"
#include <core-lib/receiver.h>
#include <string.h>

int read_request(int fd, struct state_object * so, struct http_request * req) {
    char method_str[5];
    struct receiver receiver;
    receiver_init(&receiver, fd);
    uint32_t method_str_size = sizeof(method_str) - 1;
    enum read_fully_result read_fully_result = receiver_read_until(&receiver, method_str, &method_str_size, ' ');

    switch(read_fully_result) {
        case READ_FULLY_FAILURE:
            return READ_REQUEST_ERROR;
        case READ_FULLY_MORE_DATA:
            // TODO: set error code to "Bad request"
            return READ_REQUEST_ERROR;
        case READ_FULLY_EOF:
            return READ_REQUEST_EOF;
        case READ_FULLY_SUCCESS:
        default:
            ;
            // Nothing
    }
    method_str[method_str_size] = '\0';

    if (strcmp(method_str, "GET") == 0) {
        req->method = HTTP_METHOD_GET;
    } else if (strcmp(method_str, "POST") == 0) {
        req->method = HTTP_METHOD_POST;
    } else if (strcmp(method_str, "HEAD") == 0) {
        req->method = HTTP_METHOD_HEAD;
    } else {
        // unsupported method
        // TODO: set response error code, make sure to respond later
        return READ_REQUEST_ERROR;
    }

    char ch;
    read_fully_result = receiver_read(&receiver, &ch, 1);
    // TODO: too much boilerplate
    if (read_fully_result == READ_FULLY_FAILURE) {
        return READ_REQUEST_ERROR;
    } else if (read_fully_result == READ_FULLY_EOF) {
        return READ_REQUEST_EOF;
    }
    if (ch != ' ') {
        // Invalid request: method must be followed by a space
        return READ_REQUEST_ERROR;
    }

    method_str_size = sizeof(req->request_uri) - 1;
    read_fully_result = receiver_read_until(&receiver, req->request_uri, &method_str_size, ' ');
    // TODO: too much duplicate code below
    switch(read_fully_result) {
        case READ_FULLY_FAILURE:
            return READ_REQUEST_ERROR;
        case READ_FULLY_MORE_DATA:
            // TODO: set error code to "Bad request"
            return READ_REQUEST_ERROR;
        case READ_FULLY_EOF:
            return READ_REQUEST_EOF;
        case READ_FULLY_SUCCESS:
        default:
            ;
            // Nothing
    }
    req->request_uri[method_str_size] = '\0';

    return READ_REQUEST_SUCCESS;
}

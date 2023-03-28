#include "request.h"
#include <core-lib/receiver.h>
#include <string.h>

int read_request(int fd, struct state_object * so, struct http_request * req) {
    char method_str[5];
    method_str[sizeof(method_str) - 1] = '\0';

    struct receiver receiver;
    receiver_init(&receiver, fd);
    int read_fully_result = receiver_read(&receiver, method_str, sizeof(method_str) - 1);

    // int read_fully_result = read_fully(fd, method_str, sizeof(method_str) - 1);
    if (read_fully_result == READ_FULLY_FAILURE) {
        return READ_REQUEST_ERROR;
    } else if (read_fully_result == READ_FULLY_EOF) {
        return READ_REQUEST_EOF;
    }

    char ch = ' ';
    if (strcmp(method_str, "GET ") == 0) {
        req->method = HTTP_METHOD_GET;
    } else if (strcmp(method_str, "POST") == 0) {
        req->method = HTTP_METHOD_POST;
        read_fully_result = receiver_read(&receiver, &ch, 1);
    } else if (strcmp(method_str, "HEAD") == 0) {
        req->method = HTTP_METHOD_HEAD;
        read_fully_result = receiver_read(&receiver, &ch, 1);
    } else {
        // unsupported method
        // TODO: set response error code, make sure to respond later
        return READ_REQUEST_ERROR;
    }

    if (read_fully_result == READ_FULLY_FAILURE) {
        return READ_REQUEST_ERROR;
    } else if (read_fully_result == READ_FULLY_EOF) {
        return READ_REQUEST_EOF;
    }
    if (ch != ' ') {
        // Invalid request: method must be followed by a space
        return READ_REQUEST_ERROR;
    }

    return READ_REQUEST_SUCCESS;
}

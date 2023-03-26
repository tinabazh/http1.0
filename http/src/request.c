#include "request.h"
#include <core-lib/util.h>
#include <string.h>

int read_request(int fd, struct state_object * so, struct http_request * req) {
    char method_str[5];
    method_str[sizeof(method_str) - 1] = '\0';
    int read_fully_result = read_fully(fd, method_str, sizeof(method_str) - 1);
    if (read_fully_result == READ_FULLY_FAILURE) {
        return READ_REQUEST_ERROR;
    } else if (read_fully_result == READ_FULLY_EOF) {
        return READ_REQUEST_EOF;
    }

    if (strcmp(method_str, "GET ") == 0) {
        // printf("Received GET\n");
    } else if (strcmp(method_str, "POST") == 0) {
        // printf("Received POST\n");
    } else if (strcmp(method_str, "HEAD") == 0) {
        // printf("Received HEAD\n");
    } else {
        // unsupported method
        // TODO: set response error code, make sure to respond later
        return READ_REQUEST_ERROR;
    }

    return READ_REQUEST_SUCCESS;
}

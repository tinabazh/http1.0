#include "request.h"
#include <core-lib/receiver.h>
#include <string.h>

#define HTTP_VERSION "HTTP/1.1"

enum read_request_result read_with_delim(char * buff, struct receiver *receiver, uint32_t buff_size, char delim){
    uint32_t str_size = buff_size - 1;
    enum read_fully_result read_fully_result = receiver_read_until(receiver, buff, &str_size, delim);

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
    buff[str_size] = '\0';

    char ch;
    read_fully_result = receiver_read(receiver, &ch, 1);
    // no need to check there is definitely a space. Ensured by receiver_read_until that returns success when it meets delim.
    return READ_REQUEST_SUCCESS;
}

enum read_request_result read_request(int fd, struct state_object * so, struct http_request * req) {
    char method_str[5];
    struct receiver receiver;
    receiver_init(&receiver, fd);

    enum read_request_result read_delim = read_with_delim(method_str, &receiver,
            sizeof (method_str)/ sizeof (method_str[0]), ' ');
    if (read_delim != READ_REQUEST_SUCCESS){
        return read_delim;
    }
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

    enum read_request_result read_fully_result = read_with_delim(&req->request_uri, &receiver, sizeof(req->request_uri), ' ');
    if (read_fully_result != READ_REQUEST_SUCCESS){
        return read_fully_result;
    }

    char http_version [9]; // 8 + 1 for \0
    read_fully_result = read_with_delim(http_version, &receiver, sizeof(http_version), '\x0d');
    if (read_fully_result != READ_REQUEST_SUCCESS){
        return read_fully_result;
    }
    if (strcmp(http_version, HTTP_VERSION) != 0){
        // TODO set response error code
        return READ_REQUEST_ERROR;
    }

    return READ_REQUEST_SUCCESS;
}

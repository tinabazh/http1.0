#include "request.h"
#include <core-lib/receiver.h>
#include <string.h>

#define HTTP_VERSION_1_0 "HTTP/1.0"
#define HTTP_VERSION_1_1 "HTTP/1.1"

static enum read_request_result convert_read_fully_result(enum read_fully_result result) {
    switch(result) {
        case READ_FULLY_FAILURE:
            return READ_REQUEST_INTERNAL_ERROR;
        case READ_FULLY_UNEXPECTED_RESULT:
            return READ_REQUEST_BAD_REQUEST;
        case READ_FULLY_EOF:
            return READ_REQUEST_EOF;
        case READ_FULLY_SUCCESS:
            return READ_REQUEST_SUCCESS;
    }
}

enum read_request_result read_with_delim(char * buff, struct receiver *receiver, uint32_t buff_size, char delim){
    uint32_t str_size = buff_size - 1;
    enum read_fully_result read_fully_result = receiver_read_until(receiver, buff, &str_size, delim);

    enum read_request_result result = convert_read_fully_result(read_fully_result);
    if (result != READ_REQUEST_SUCCESS) {
        return result;
    }
    buff[str_size] = '\0';

    char ch;
    read_fully_result = receiver_read(receiver, &ch, 1);
    // no need to check there is definitely a space. Ensured by receiver_read_until that returns success when it meets delim.
    return READ_REQUEST_SUCCESS;
}

/**
 * Extracts the next character and checks if it's expected
 * @return
 */
enum read_fully_result receiver_ensure_char(struct receiver * receiver, char expected) {
    char ch;
    enum read_fully_result read_fully_result = receiver_read(receiver, &ch, 1);
    if (read_fully_result != READ_FULLY_SUCCESS) {
        return read_fully_result;
    }
    return ch == expected ? READ_FULLY_SUCCESS : READ_FULLY_UNEXPECTED_RESULT;
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
        return READ_REQUEST_BAD_REQUEST;
    }

    enum read_request_result read_req_result = read_with_delim(req->request_uri, &receiver, sizeof(req->request_uri), ' ');
    if (read_req_result != READ_REQUEST_SUCCESS){
        return read_req_result;
    }

    char http_version [9]; // 8 + 1 for \0
    read_req_result = read_with_delim(http_version, &receiver, sizeof(http_version), '\r');
    if (read_req_result != READ_REQUEST_SUCCESS){
        return read_req_result;
    }
    if (strcmp(http_version, HTTP_VERSION_1_0) != 0 && strcmp(http_version, HTTP_VERSION_1_1) != 0){
        return READ_REQUEST_BAD_REQUEST;
    }

    {
        enum read_fully_result read_fully_result = receiver_ensure_char(&receiver, '\n');
        if (read_fully_result != READ_FULLY_SUCCESS){
            return convert_read_fully_result(read_fully_result);
        }
    }

    return READ_REQUEST_SUCCESS;
}

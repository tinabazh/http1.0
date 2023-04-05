#include "handlers.h"
#include "response.h"
#include "request.h"
#include <string.h>
#include <unistd.h>

bool handle_request(enum read_request_result read_request_result, struct http_request * req, int fd) {
    // TODO handle EOF in serve and write
    // TODO check HTTP method - this is for GET only.
    if (read_request_result == READ_REQUEST_SUCCESS) {
        if (req->method == HTTP_METHOD_GET || req->method == HTTP_METHOD_HEAD) {
            return (serve_file(req->request_uri, fd, req->method == HTTP_METHOD_GET));
        } else {
            return(write_status_line(RESPONSE_RESULT_METHOD_NOT_IMPLEMENTED, fd) && write_content_length(0, fd));
        }
    } else if (read_request_result == READ_REQUEST_BAD_REQUEST) {
        return(write_status_line(RESPONSE_RESULT_BAD_REQUEST, fd) && write_content_length(0, fd) );
    }
    return false;
}

enum pollin_handle_result pollin_handle_http(struct core_object *co, struct state_object *so, int fd) {
    struct http_request req;
    memset(&req, 0, sizeof(req));
    enum read_request_result read_request_result = read_request(fd, so, &req);

    if (read_request_result == READ_REQUEST_SUCCESS || read_request_result == READ_REQUEST_BAD_REQUEST) {
        if (handle_request(read_request_result, &req, fd) == false) {
            return POLLIN_HANDLE_RESULT_FATAL;
        } else {
            int close_result = close(fd);
            return close_result == 0 ? POLLIN_HANDLE_RESULT_EOF : POLLIN_HANDLE_RESULT_FATAL;
        }
    }
    return read_request_result == READ_REQUEST_EOF ? POLLIN_HANDLE_RESULT_EOF : POLLIN_HANDLE_RESULT_FATAL;
}
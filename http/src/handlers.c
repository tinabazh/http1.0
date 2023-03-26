#include "handlers.h"
#include <http/request.h>
#include <string.h>

void handle_request(struct core_object * co) {
}

// return -1 in case of error
int write_response(int fd, struct state_object * so) {
    return 0;
}

int pollin_handle_http(struct core_object *co, struct state_object *so, int fd) {
    struct http_request req;
    memset(&req, 0, sizeof(req));
    const int read_request_result = read_request(fd, so, &req);
    if (read_request_result == READ_REQUEST_SUCCESS) {
        handle_request(co);
    }
    if (read_request_result != READ_REQUEST_EOF) {
        if (write_response(fd, so) == -1) {
            return POLLIN_HANDLE_RESULT_FATAL;
        }
    }
    return read_request_result == READ_REQUEST_EOF ? POLLIN_HANDLE_RESULT_EOF : POLLIN_HANDLE_RESULT_OK;
}
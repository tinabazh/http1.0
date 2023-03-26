#ifndef POLL_SERVER_READ_H
#define POLL_SERVER_READ_H

#include <objects.h>

enum read_request_result {
    READ_REQUEST_SUCCESS,
    READ_REQUEST_ERROR,
    READ_REQUEST_EOF,
};

int read_request(int fd, struct state_object * so);

#endif //POLL_SERVER_READ_H

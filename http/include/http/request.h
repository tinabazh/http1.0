#ifndef HTTPSERVER_REQUEST_H
#define HTTPSERVER_REQUEST_H

#include <core-lib/objects.h>

struct http_request {

};

enum read_request_result {
    READ_REQUEST_SUCCESS,
    READ_REQUEST_ERROR,
    READ_REQUEST_EOF,
};

// returns read_request_result
int read_request(int fd, struct state_object * so, struct http_request * req);

#endif //HTTPSERVER_REQUEST_H

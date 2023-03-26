#ifndef HTTPSERVER_HANDLERS_H
#define HTTPSERVER_HANDLERS_H

#include <core-lib/objects.h>

int read_request_http(int fd, struct state_object * so);
void handle_request_http(struct core_object * co);
int write_response_http(int fd, struct state_object * so);

#endif //HTTPSERVER_HANDLERS_H

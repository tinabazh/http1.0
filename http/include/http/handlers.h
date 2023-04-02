#ifndef HTTPSERVER_HANDLERS_H
#define HTTPSERVER_HANDLERS_H

#include <core-lib/objects.h>

enum pollin_handle_result pollin_handle_http(struct core_object *co, struct state_object *so, int fd);

#endif //HTTPSERVER_HANDLERS_H

#ifndef SCALABLE_SERVER_OBJECTS_H
#define SCALABLE_SERVER_OBJECTS_H

#include <netinet/in.h>
#include <stdio.h>

struct core_object;
struct state_object;

typedef int (*read_request_handler)(int fd, struct state_object * so);
typedef void (*handle_request_handler)(struct core_object * co);
typedef int (*write_response_handler)(int fd, struct state_object * so);

struct handlers {
    read_request_handler read_request;
    handle_request_handler handle_request;
    write_response_handler write_response;
};

/**
 * core_object
 * <p>
 * Holds the core information for the execution of the framework, regardless
 * of the library loaded. Includes dc_env, dc_error, memory_manager, log file,
 * and state_object. state_object contains library-dependent data, and will be
 * assigned and handled by the loaded library.
 * </p>
 */
struct core_object {
    struct memory_manager *mm;
    FILE *log_file;
    struct sockaddr_in listen_addr;
    struct state_object *so;
    struct handlers handlers;
};

#endif //SCALABLE_SERVER_OBJECTS_H

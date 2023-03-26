#ifndef SCALABLE_SERVER_OBJECTS_H
#define SCALABLE_SERVER_OBJECTS_H

#include <netinet/in.h>
#include <stdio.h>

struct core_object;
struct state_object;
struct pollfd;

enum pollin_handle_result {
    POLLIN_HANDLE_RESULT_OK,
    POLLIN_HANDLE_RESULT_EOF,
    POLLIN_HANDLE_RESULT_FATAL,
};

// returns pollin_handle_result
typedef int (*pollin_handler)(struct core_object *co, struct state_object *so, int fd);

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
    pollin_handler pollin_handler;
};

#endif //SCALABLE_SERVER_OBJECTS_H

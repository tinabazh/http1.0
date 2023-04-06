#ifndef SCALABLE_SERVER_POLL_OBJECTS_H
#define SCALABLE_SERVER_POLL_OBJECTS_H

#include <netinet/in.h>

/**
 * The maximum number of connections that can be accepted by the poll server.
 */
#define MAX_CONNECTIONS 5

struct state_object {
    int listen_fd;
    int client_fd[MAX_CONNECTIONS];
    struct sockaddr_in client_addr[MAX_CONNECTIONS];
    size_t num_connections;
};

#endif //SCALABLE_SERVER_POLL_OBJECTS_H

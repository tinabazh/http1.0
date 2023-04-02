#ifndef SCALABLE_SERVER_POLL_OBJECTS_H
#define SCALABLE_SERVER_POLL_OBJECTS_H

#include <core-lib/objects.h>
#include <response.h>

#include <ndbm.h>
#include <stdbool.h>

/**
 * The maximum number of connections that can be accepted by the poll server.
 */
#define MAX_CONNECTIONS 5

/**
 * The number of connections that can be queued on the listening socket.
 */
#define CONNECTION_QUEUE 100

/**
 * The maximum length of a display name or any other sort of name.
 */
#define MAX_NAME_LEN 20

/**
 * The maximum length of an IP address in string form.
 */
#define MAX_IP_LEN 21

/**
 * Possible actions a client may request.
 */
enum action{CREATE = 1, READ = 2, UPDATE = 3, DELETE = 4};

/**
 * Possible objects a request may operate on.
 */
enum object{USER = 1, CHANNEL = 2, MESSAGE = 3, AUTH = 4};

/**
 * Represents the header of a message. Comments indicate how big the field is in the RFC.
 */
struct header {
    int version; // 4 bits
    enum action action; // 4 bits
    enum object object; // 8 bits
    uint16_t body_size; // 16 bits
};


/**
 * Represents a connection to the server as an active user (not to be confused with the user DB).
 */
struct active_user {
    bool occupied; // used for inserting a new user into active_users
    char display_name[MAX_NAME_LEN + 1];
    char ip[MAX_IP_LEN + 1];
};

/**
 * Represents connections to the server as a list of active users.
 */
struct active_users {
    struct active_user users[MAX_CONNECTIONS];
};

struct state_object {
    int listen_fd;
    int client_fd[MAX_CONNECTIONS];
    struct sockaddr_in client_addr[MAX_CONNECTIONS];
    size_t num_connections;
    struct active_users active_users;
};

#endif //SCALABLE_SERVER_POLL_OBJECTS_H

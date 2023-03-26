#include <handle.h>
#include <uuid/uuid.h>
#include "database.h"
#define UUID_LENGTH 37

static void perform_action(struct core_object * co);

void handle_request(struct core_object * co) {
    // TODO create more functions and mimic the handle state machine here
    perform_action(co);
}

static void perform_action_CREATE(struct core_object * co) {
    switch (co->so->req->header.object) {
        case USER:
            create_user(co);
            break;
        case MESSAGE: {
            uuid_t uuid;
            char uuid_str[UUID_LENGTH];
            uuid_generate(uuid);
            // Convert the binary UUID to a string representation
            uuid_unparse(uuid, uuid_str);
            printf("Generated UUID in a message: %s\n", uuid_str);
            int result = create_message(co, uuid_str);
            if (result == 0)
                co->so->broadcast = true;
            break;
        }
        case CHANNEL: {
            uuid_t uuid;
            char uuid_str[UUID_LENGTH];
            uuid_generate(uuid);
            // Convert the binary UUID to a string representation
            uuid_unparse(uuid, uuid_str);
            printf("Generated UUID in a channel: %s\n", uuid_str);
            create_channel(co, uuid_str);
            break;
        }
        case AUTH:
            create_auth(co);
            break;
        default:
            fprintf(stderr, "unknown object\n");
    }
}

static void perform_action(struct core_object * co) {
    // TODO perform the action specified in the message
    // for example if the request was CREATE and MESSAGE, call the create_message db function

    switch (co->so->req->header.action) {
        case CREATE:
            perform_action_CREATE(co);
            break;
        case READ:
            break;
        case UPDATE:
            break;
        case DELETE:
            break;
        default:
            fprintf(stderr, "unknown action\n");
    }
}



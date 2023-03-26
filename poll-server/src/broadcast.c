#include <broadcast.h>

int write_broadcast(struct core_object * co, struct state_object * so) {
    // TODO create functions and mimic the broadcast state diagram here
    // - this sends out either a PING request or a CREATE_MESSAGE request
    // - broadcast will need so.req and so.res free so that it can create a request and read responses but it also
    //   needs to read the previous request to determine whether to send a PING or CREATE_MESSAGE request. It will
    //   also need to read the previous request to determine what was updated for PING
    //   my thinking is to keep the previous request allocated, construct the new request without setting it in the state object,
    //   and then once the new request is fully constructed it can replace the old one in the state object.
    return 0;
}

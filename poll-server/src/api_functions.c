#include <core-lib/api_functions.h>
#include "../include/poll_server.h"
#include "database.h"

int initialize_server(struct core_object *co)
{
    printf("INIT POLL SERVER\n");
    
    co->so = setup_poll_state(co->mm);
    if (!co->so)
    {
        return ERROR;
    }

    int db_init = init_db(co);
    if(db_init == -1){
        close_db(co);
    }

    if (open_poll_server_for_listen(co, co->so, &co->listen_addr) == -1)
    {
        return ERROR;
    }
    
    return RUN_SERVER;
}

int run_server(struct core_object *co)
{
    printf("RUN POLL SERVER\n");
    
    if (run_poll_server(co) == -1)
    {
        return ERROR;
    }
    
    return CLOSE_SERVER;
}

int close_server(struct core_object *co)
{
    printf("CLOSE POLL SERVER\n");

    close_db(co);
    destroy_poll_state(co, co->so);
    
    return EXIT;
}

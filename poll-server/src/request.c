#include <request.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct request * init_request(void) {
    struct request * req = malloc(sizeof(struct request));
    if (req == NULL) {
        perror("malloc request");
        return NULL;
    }
    memset(req, 0, sizeof(struct request));

    return req;
}

void destroy_request(struct request ** req) {
    if (*req != NULL) {
        if ((*req)->raw_body != NULL) {
            free((*req)->raw_body);
        }
        if ((*req)->sender_ip != NULL) {
            free((*req)->sender_ip);
        }
        destroy_create_user_req(&(*req)->create_user);
        destroy_create_channel_req(&(*req)->create_channel);
        destroy_create_message_req(&(*req)->create_message);
        destroy_create_auth_req(&(*req)->create_auth);
        free(*req);
    }
    *req = NULL;
}

struct create_user_req * init_create_user_req(char * login_token, char * display_name, char * password) {
    struct create_user_req * cu_req = malloc(sizeof(struct create_user_req));
    if (cu_req == NULL) {
        perror("malloc create user request");
        return NULL;
    }
    memset(cu_req, 0, sizeof (struct create_user_req));

    cu_req->login_token = strdup(login_token);
    if (cu_req->login_token == NULL) {
        perror("malloc create user request login token");
        return NULL;
    }
    cu_req->display_name = strdup(display_name);
    if (cu_req->display_name == NULL) {
        perror("malloc create user request display name");
        return NULL;
    }
    cu_req->password = strdup(password);
    if (cu_req->password == NULL) {
        perror("malloc create user request password");
        return NULL;
    }

    return cu_req;
}

void destroy_create_user_req(struct create_user_req ** cu_req) {
    if (*cu_req != NULL) {
        if ((*cu_req)->login_token != NULL)
        {
            free((*cu_req)->login_token);
        }
        if ((*cu_req)->display_name != NULL)
        {
            free((*cu_req)->display_name);
        }
        if ((*cu_req)->password != NULL)
        {
            free((*cu_req)->password);
        }
        free(*cu_req);
    }
    *cu_req = NULL;
}

struct create_channel_req * init_create_channel_req(char * channel_name, char * display_name, bool public) {
    struct create_channel_req * cc_req = malloc(sizeof(struct create_channel_req));
    if (cc_req == NULL) {
        perror("malloc create channel request");
        return NULL;
    }
    memset(cc_req, 0, sizeof (struct create_channel_req));

    cc_req->channel_name = strdup(channel_name);
    if (cc_req->channel_name == NULL) {
        perror("malloc create channel request channel name");
        return NULL;
    }
    cc_req->display_name = strdup(display_name);
    if (cc_req->display_name == NULL) {
        perror("malloc create channel request display name");
        return NULL;
    }
    cc_req->public = public;

    return  cc_req;
}

void destroy_create_channel_req(struct create_channel_req ** cc_req) {
    if (*cc_req != NULL) {
        if ((*cc_req)->channel_name != NULL) {
            free((*cc_req)->channel_name);
        }
        if ((*cc_req)->display_name != NULL) {
            free((*cc_req)->display_name);
        }
        free(*cc_req);
    }
    *cc_req = NULL;
}

struct create_message_req * init_create_message_req(char * display_name, char * channel_name, char * message_content, uint64_t timestamp) {
    struct create_message_req * cm_req = malloc(sizeof(struct create_message_req));
    if (cm_req == NULL) {
        perror("malloc create message request");
        return NULL;
    }
    memset(cm_req, 0, sizeof (struct create_message_req));

    cm_req->display_name = strdup(display_name);
    if (cm_req->display_name == NULL) {
        perror("malloc create message request display name");
        return NULL;
    }
    cm_req->channel_name = strdup(channel_name);
    if (cm_req->channel_name == NULL) {
        perror("malloc create message request channel name");
        return NULL;
    }
    cm_req->message_content = strdup(message_content);
    if (cm_req->message_content == NULL) {
        perror("malloc create message request message content");
        return NULL;
    }
    cm_req->timestamp = timestamp;

    return cm_req;
}

void destroy_create_message_req(struct create_message_req ** cm_req) {
    if (*cm_req != NULL) {
        if ((*cm_req)->channel_name != NULL) {
            free((*cm_req)->channel_name);
        }
        if ((*cm_req)->display_name != NULL) {
            free((*cm_req)->display_name);
        }
        if ((*cm_req)->message_content != NULL) {
            free((*cm_req)->message_content);
        }
        free(*cm_req);
    }
    *cm_req = NULL;
}

struct create_auth_req * init_create_auth_req(char * login_token, char * password) {
    struct create_auth_req * ca_req = malloc(sizeof(struct create_auth_req));
    if (ca_req == NULL) {
        perror("malloc create auth request");
        return NULL;
    }
    memset(ca_req, 0, sizeof (struct create_user_req));

    ca_req->login_token = strdup(login_token);
    if (ca_req->login_token == NULL) {
        perror("maloc create auth request login token");
        return NULL;
    }
    ca_req->password = strdup(password);
    if (ca_req->password == NULL) {
        perror("maloc create auth request password");
        return NULL;
    }

    return ca_req;
}

void destroy_create_auth_req(struct create_auth_req ** ca_req) {
    if (*ca_req != NULL) {
        if ((*ca_req)->login_token != NULL) {
            free((*ca_req)->login_token);
        }
        if ((*ca_req)->password != NULL) {
            free((*ca_req)->password);
        }
        free(*ca_req);
    }
    *ca_req = NULL;
}

#ifndef POLL_SERVER_REQUEST_H
#define POLL_SERVER_REQUEST_H

#include <objects.h>

#include <stdbool.h>
#include <stdint.h>

// REQUEST //

/**
 * init_request
 * <p>
 * Initializes and zeros out a request struct.
 * Free with destroy_request().
 * </p>
 * @return struct pointer on success, NULL and set errno on failure.
 */
struct request * init_request(void);

/**
 * destroy_request
 * <p>
 * Frees all dynamically allocated memory in a request struct. This includes any request type structs.
 * </p>
 * @param req double pointer to the request being destroyed.
 */
void destroy_request(struct request ** req);

// CREATE //

/**
 * Represents fields the client sends in a create user request.
 */
struct create_user_req {
    // data to be read
    char * login_token;
    char * display_name;
    char * password;

    // data needed to respond
    int non_unique_field_specifier; // if a non-unique error occurred while handling
};

/**
 * Represents fields the client sends in a create channel request.
 */
struct create_channel_req {
    char * channel_name;
    char * display_name;
    bool public;
};

/**
 * Represents fields the client sends in a create message request.
 */
struct create_message_req {
    // data to be read
    char * display_name;
    char * channel_name;
    char * message_content;
    uint64_t timestamp; // 64 bits

    // data needed to respond
    bool global_admin_404;
};

/**
 * Represents fields the client sends in a create message request.
 */
struct create_auth_req {
    char * login_token;
    char * password;
};

/**
 * init_create_user_req
 * <p>
 * Initializes a create user request. This involves allocating memory and setting fields to desired values.
 * Free with destroy_create_user_req().
 * </p>
 * @param login_token the new users login token.
 * @param display_name the new users display name.
 * @param password the new users password.
 * @return a struct pointer on success, NULL and set errno on failure.
 */
struct create_user_req * init_create_user_req(char * login_token, char * display_name, char * password);

/**
 * destroy_create_user_req
 * <p>
 * Free any dynamically allocated memory from a create user request.
 * </p>
 * @param cu_req double pointer to the request being destroyed.
 */
void destroy_create_user_req(struct create_user_req ** cu_req);

/**
 * init_create_channel_req
 * <p>
 * Initializes a create channel request. This involves allocating memory and setting fields to desired values.
 * Free with destroy_create_channel_req().
 * </p>
 * @param channel_name the new channels name.
 * @param display_name the requesting users display name.
 * @param public if the channel should be public.
 * @return a struct pointer on success, NULL and set errno on failure.
 */
struct create_channel_req * init_create_channel_req(char * channel_name, char * display_name, bool public);

/**
 * destroy_create_channel_req
 * <p>
 * Free any dynamically allocated memory from a create channel request.
 * </p>
 * @param cc_req double pointer to the request being destroyed.
 */
void destroy_create_channel_req(struct create_channel_req ** cc_req);

/**
 * init_create_message_req
 * <p>
 * Initializes a create message request. This involves allocating memory and setting fields to desired values.
 * Free with destroy_create_message_req().
 * </p>
 * @param display_name the display name of the requesting user.
 * @param channel_name the name of the channel to send the message to.
 * @param message_content the message contents.
 * @param timestamp timestamp the message was sent at.
 * @return a struct pointer on success, NULL and set errno on failure.
 */
struct create_message_req * init_create_message_req(char * display_name, char * channel_name, char * message_content,
        uint64_t timestamp);

/**
 * destroy_create_message_req
 * <p>
 * Free any dynamically allocated memory from a create message request.
 * </p>
 * @param cm_req double pointer to the request being destroyed.
 */
void destroy_create_message_req(struct create_message_req ** cm_req);

/**
 * init_create_auth_req
 * <p>
 * Initializes a create auth request. This involves allocating memory and setting fields to desired values.
 * Free with destroy_create_auth_req().
 * </p>
 * @param login_token the requesting users login token.
 * @param password the requesting users password.
 * @return a struct pointer on success, NULL and set errno on failure.
 */
struct create_auth_req * init_create_auth_req(char * login_token, char * password);

/**
 * destroy_create_auth_req
 * <p>
 * Free any dynamically allocated memory from a create auth request.
 * </p>
 * @param ca_req double pointer to the request being destroyed.
 */
void destroy_create_auth_req(struct create_auth_req ** ca_req);

#endif //POLL_SERVER_REQUEST_H

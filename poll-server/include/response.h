#ifndef POLL_SERVER_RESPONSE_H
#define POLL_SERVER_RESPONSE_H

#include <inttypes.h>
#include <objects.h>

// RESPONSE //

/**
 * init_response
 * <p>
 * Initializes and zeros out a response struct.
 * Free with destroy_request().
 * </p>
 * @return struct pointer on success, NULL and set errno on failure.
 */
struct response * init_response(void);

/**
 * destroy_response
 * <p>
 * Frees all dynamically allocated memory in a response struct. This includes any response type structs.
 * </p>
 * @param res double pointer to the response being destroyed.
 */
void destroy_response(struct response ** res);

// STANDARD RESPONSES //

/**
 * Represents fields to respond with in a standard error response (excluding response code).
 */
struct standard_error_res {
    char * error_message;
};

/**
 * rawify_standard_response
 * <p>
 * Transforms a response code into a raw body.
 * </p>
 * @param code the response code.
 * @param body_dst where to set the raw body.
 * @return 0 on success, -1 and set errno on failure.
 */
int rawify_standard_response(int code, char ** body_dst);

/**
 * init_standard_error_res
 * <p>
 * Initializes a standard error response. This involves allocating memory and setting fields to desired values.
 * Free with destroy_standard_error_res().
 * </p>
 * @param error_message the error message to send.
 * @return struct pointer on success, NULL and set errno on failure.
 */
struct standard_error_res * init_standard_error_res(char * error_message);

/**
 * rawify_standard_error_res
 * <p>
 * Transforms a standard_error_res struct into a raw body.
 * </p>
 * @param code the response code.
 * @param body_dst where to allocate and set the raw body.
 * @param sa_res the response to transform.
 * @return 0 on success, -1 and set errno on failure.
 */
int rawify_standard_error_res(int code, char ** body_dst, struct standard_error_res * sa_res);

/**
 * destroy_standard_error_res
 * <p>
 * Free any dynamically allocated memory from a standard error response.
 * </p>
 * @param se_res double pointer to the response being destroyed.
 */
void destroy_standard_error_res(struct standard_error_res ** se_res);

// CREATE RESPONSES //

/**
 * Represents fields to respond with in a create user response (excluding response code).
 */
struct create_user_res {
    // No fields
};

/**
 * Represents fields to respond with in a create channel response (excluding response code).
 */
struct create_channel_res {
    // No fields
};

/**
 * Represents fields to respond with in a create message response (excluding response code).
 */
struct create_message_res {
    // No fields
};

/**
 * Represents fields to respond with in a create auth response (excluding response code).
 */
struct create_auth_res {
    char * display_name;
    int  privilege_level; // -1, 0 or 1
    int channel_name_size; // how many channel names in the list, not sent
    char ** channel_name_list;
};

/**
 * Represents fields to respond with in a create message response (excluding response code).
 */
struct create_user_non_unique_error_res {
    int field_specifier;
    char * error_message;
};

/**
 * init_create_auth_res
 * <p>
 * Initializes a create auth response. This involves allocating memory and setting fields to desired values.
 * IMPORTANT: channel_name_list must be allocated BEFORE being passed to this function.
 * Free with destroy_create_auth_res().
 * </p>
 * @param display_name the display name of the requesting user.
 * @param privilege_level the privilege level of the requesting user.
 * @param channel_name_size number of entries in channel_name_list.
 * @param channel_name_list an array of channel names.
 * @return struct pointer on success, NULL and set errno on failure.
 */
struct create_auth_res * init_create_auth_res(char * display_name, int privilege_level, int channel_name_size, char ** channel_name_list);

/**
 * rawify_create_auth_res
 * <p>
 * Transforms a create_auth_res struct into a raw body.
 * </p>
 * @param code the response code.
 * @param body_dst where to allocate and set the raw body.
 * @param ca_res the response to transform.
 * @return 0 on success, -1 and set errno on failure.
 */
int rawify_create_auth_res(int code, char ** body_dst, struct create_auth_res * ca_res);

/**
 * destroy_create_auth_res
 * <p>
 * Free any dynamically allocated memory from a create auth response.
 * </p>
 * @param ca_res double pointer to the response being destroyed.
 */
void destroy_create_auth_res(struct create_auth_res ** ca_res);

/**
 * init_create_user_non_unique_error_res
 * <p>
 * Initializes a create user non-unique error response. This involves allocating memory and setting fields to desired values.
 * Free with destroy_create_user_non_unique_error_res().
 * </p>
 * @param field_specifier the field where the error occurred (see RFC).
 * @param error_message the error message to send.
 * @return struct pointer on success, NULL and set errno on failure.
 */
struct create_user_non_unique_error_res * init_create_user_non_unique_error_res(int field_specifier, char * error_message);

/**
 * rawify_create_user_non_unique_error_res
 * <p>
 * Transforms a create_user_non_unique_error_res struct into a raw body.
 * </p>
 * @param code the response code.
 * @param body_dst where to allocate and set the raw body.
 * @param cunue_res the response to transform.
 * @return 0 on success, -1 and set errno on failure.
 */
int rawify_create_user_non_unique_error_res(int code, char ** body_dst, struct create_user_non_unique_error_res * cunue_res);

/**
 * destroy_standard_error_res
 * <p>
 * Free any dynamically allocated memory from a create user non-unique error response.
 * </p>
 * @param cunue_res double pointer to the response being destroyed.
 */
void destroy_create_user_non_unique_error_res(struct create_user_non_unique_error_res ** cunue_res);

#endif //POLL_SERVER_RESPONSE_H

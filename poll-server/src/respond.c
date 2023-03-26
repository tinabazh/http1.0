#include <respond.h>

#include <util.h>

#include <string.h>

#define VERSION 1

/**
 * get_result_code
 * <p>
 * Gets the result code from the request and sets it in the response.
 * </p>
 * @param so the state object.
 * @return 0 on success, -1 on failure.
 */
static int get_result_code(struct state_object * so);

/**
 * create_res_body
 * <p>
 * Creates a response based on the request action and object and rawifies it.
 * </p>
 * @param so the state object.
 * @return 0 on success, -1 and set errno on failure.
 */
static int create_res_body(struct state_object * so);

/**
 * create_res_body_CREATE
 * <p>
 * Creates a response for CREATE requests and rawifies it.
 * </p>
 * @param so the state object.
 * @return 0 on success, -1 and set errno on failure.
 */
static int create_res_body_CREATE(struct state_object * so);

/**
 * create_header
 * <p>
 * Creates a header and rawifies it (does not convert to network order).
 * </p>
 * @param version the protocol version.
 * @param action the action being performed.
 * @param object the object being operated on.
 * @param body_size the size of the message body.
 * @param h pointer to a header struct.
 * @param raw_header pointer to where to assign the raw header.
 * @return 0 on success, -1 and set errno on failure.
 */
int create_header(int version, enum action action, enum object object, uint16_t body_size, struct header * h, uint32_t * raw_header);

/**
 * write_res
 * <p>
 * Writes a response to a file descriptor. A header is written first (converted to network order),
 * and then the raw body is written.
 * </p>
 * @param fd the file descriptor to write to.
 * @param so the state object.
 * @return 0 on success, -1 and set errno on failure.
 */
static int write_res(int fd, struct state_object * so);

int write_response(int fd, struct state_object * so) {
    so->res = init_response();
    if (so->res == NULL) {
        return -1;
    }
    if (get_result_code(so) == -1) {
        return -1;
    }
    if (create_res_body(so) == -1) {
        return -1;
    }
    if (create_header(VERSION, so->req->header.action, so->req->header.object, strlen(so->res->raw_body), &so->res->header, &so->res->raw_header) == -1) {
        return -1;
    }
    if (write_res(fd, so) == -1) {
        return -1;
    }

    return 0;
}

static int get_result_code(struct state_object * so) {
    if (so->res == NULL) {
        (void) fprintf(stderr, "get_result_code: response struct not allocated\n");
        return -1;
    }
    if (so->req == NULL) {
        (void) fprintf(stderr, "get_result_code: request struct not allocated\n");
        return -1;
    }
    so->res->result_code = so->req->result_code;

    return 0;
}

static int create_res_body(struct state_object * so) {
    switch (so->req->header.action)
    {
        case CREATE:
            return create_res_body_CREATE(so);
        case READ:
            return 0;
        case UPDATE:
            return 0;
        case DELETE:
            return 0;
        default:
            (void) fprintf(stderr, "create response body: unknown action\n");
            return -1;
    }
}

static int create_res_body_CREATE(struct state_object * so) {
    // TODO maybe functionize each response, this function is too long
    switch (so->req->header.object)
    {
        case USER:
        {
            if (so->res->result_code != REQUEST_RESULT_CREATED) {
                if (so->res->result_code == 409) {
                    // non-unique error response
                    so->res->create_user_non_unique_error = init_create_user_non_unique_error_res(
                            so->req->create_user->non_unique_field_specifier, "Create user: field not unique");
                    if (so->res->create_user_non_unique_error == NULL) {
                        return -1;
                    }
                    if (rawify_create_user_non_unique_error_res(so->res->result_code, &so->res->raw_body, so->res->create_user_non_unique_error)) {
                        return -1;
                    }
                } else {
                    // standard error response
                    char * msg;
                    switch (so->res->result_code)
                    {
                        case 400:
                            msg = "Create user: invalid field(s)";
                            break;
                        default:
                            msg = "Create user: undefined error";
                    }
                    so->res->standard_error = init_standard_error_res(msg);
                    if (so->res->standard_error == NULL) {
                        return -1;
                    }
                    if (rawify_standard_error_res(so->res->result_code, &so->res->raw_body, so->res->standard_error)) {
                        return -1;
                    }
                }
            } else {
                // standard success response
                if (rawify_standard_response(so->res->result_code, &so->res->raw_body) == -1)
                {
                    return -1;
                }
            }
            return 0;
        }
        case CHANNEL:
        {
            if (so->res->result_code != REQUEST_RESULT_CREATED) {
                // standard error response
                char * msg;
                switch (so->res->result_code)
                {
                    case 400:
                        msg = "Create channel: invalid field(s)";
                        break;
                    case 403:
                        msg = "Create channel: user not found";
                        break;
                    case 404:
                        msg = "Create channel: global admin not found";
                        break;
                    case 409:
                        msg = "Create channel: channel name already exists";
                        break;
                    default:
                        msg = "Create channel: undefined error";
                }
                so->res->standard_error = init_standard_error_res(msg);
                if (so->res->standard_error == NULL) {
                    return -1;
                }
                if (rawify_standard_error_res(so->res->result_code, &so->res->raw_body, so->res->standard_error)) {
                    return -1;
                }
            } else {
                // standard success response
                if (rawify_standard_response(so->res->result_code, &so->res->raw_body) == -1) {
                    return -1;
                }
            }
            return 0;
        }
        case MESSAGE:
        {
            if (so->res->result_code != 201) {
                // standard error response
                char * msg;
                switch (so->res->result_code)
                {
                    case 400:
                        msg = "Create message: invalid field(s)";
                        break;
                    case 403:
                        msg = "Create message: user not found";
                        break;
                    case 404:
                        if (so->req->create_message->global_admin_404) {
                            msg = "Create message: global admin not found";
                        } else {
                            msg = "Create message: channel not found";
                        }
                        break;
                    case 409:
                        msg = "Create message: channel name already exists";
                        break;
                    default:
                        msg = "Create message: undefined error";
                }
                so->res->standard_error = init_standard_error_res(msg);
                if (so->res->standard_error == NULL) {
                    return -1;
                }
                if (rawify_standard_error_res(so->res->result_code, &so->res->raw_body, so->res->standard_error)) {
                    return -1;
                }
            } else {
                // standard success response
                if (rawify_standard_response(so->res->result_code, &so->res->raw_body) == -1) {
                    return -1;
                }
            }
            return 0;
        }
        case AUTH:
        {
            if (so->res->result_code != 200) {
                char * msg;
                switch (so->res->result_code)
                {
                    case 400:
                        msg = "Create auth: invalid field(s)";
                        break;
                    case 403:
                        msg = "Create auth: user not found";
                        break;
                    default:
                        msg = "Create message: undefined error";
                }
                so->res->standard_error = init_standard_error_res(msg);
                if (so->res->standard_error == NULL) {
                    return -1;
                }
                if (rawify_standard_error_res(so->res->result_code, &so->res->raw_body, so->res->standard_error)) {
                    return -1;
                }
            } else {
                // create auth response
                so->res->create_auth = init_create_auth_res("db needs work :)", 0, 0, NULL);
                if (so->res->create_auth == NULL) {
                    return -1;
                }
                if(rawify_create_auth_res(so->res->result_code, &so->res->raw_body, so->res->create_auth) == -1) {
                    return -1;
                }
            }
            return 0;
        }
        default:
            (void) fprintf(stderr, "create CREATE response body: unknown object\n");
            return -1;
    }
}

int create_header(int version, enum action action, enum object object, uint16_t body_size, struct header * h, uint32_t * raw_header) {
    // TODO: maybe add some error checking in here
    h->version = version;
    h->action = action;
    h->object = object;
    h->body_size = body_size;

    *raw_header = 0; // explicitly set to 0 to avoid garbage values
    write_bits(raw_header, body_size, 16, 0);
    write_bits(raw_header, object, 8, 16);
    write_bits(raw_header, action, 4, 24);
    write_bits(raw_header, version, 4, 28);

    return 0;
}

static int write_res(int fd, struct state_object * so) {
    uint32_t net_header = htonl(so->res->raw_header);
    if (write_fully(fd, &net_header, sizeof(uint32_t)) == -1) {
        return -1;
    }
    if (write_fully(fd, so->res->raw_body, strlen(so->res->raw_body)) == -1) {
        return -1;
    }

    return 0;
}

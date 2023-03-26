#include <read.h>

#include <util.h>

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>


#define INIT_REQUEST_ERROR 400
#define READ_HEADER_ERROR 400
#define ALLOCATE_ERROR 400
#define SEPARATE_BODY_ERROR 400
#define VALIDATE_BODY_ERROR 400
#define MARSHAL_ERROR 400

#define DELIM '\x03'

/**
 * read_header
 * <p>
 * Reads a header from a file descriptor and marshals it into a header struct.
 * </p>
 * @param fd the file descriptor to read from.
 * @param h the header struct.
 * @return
 */
int read_header(int fd, struct header * h, uint32_t * raw_header);

/**
 * extract_version
 * <p>
 * Extracts the version number from the header.
 * </p>
 * @param header the header in binary.
 * @param h header struct to assign the version to.
 * @return 0 on success, -1 on error.
 */
static int extract_version(uint32_t header, struct header * h);

/**
 * extract_action
 * <p>
 * Extracts the action from the header.
 * </p>
 * @param header the header in binary.
 * @param h header struct to assign the action to.
 * @return 0 on success, -1 on error.
 */
static int extract_action(uint32_t header, struct header * h);

/**
 * extract_object
 * <p>
 * Extracts the object from the header.
 * </p>
 * @param header the header in binary.
 * @param h header struct to assign the object to.
 * @return 0 on success, -1 on error.
 */
static int extract_object(uint32_t header, struct header * h);

/**
 * extract_body_size
 * <p>
 * Extracts the body size from the header.
 * </p>
 * @param header the header in binary.
 * @param h header struct to assign the body size to.
 * @return 0 on success, -1 on error.
 */
static int extract_body_size(uint32_t header, struct header * h);

/**
 * read_body
 * <p>
 * Allocates memory and reads a message body from a file descriptor into a destination.
 * </p>
 * @param fd file descriptor to read from.
 * @param body_size amount to read in bytes.
 * @param body destination to read body into.
 * @return 0 on success, -1 and set errno on failure.
 */
static int read_body(int fd, uint32_t body_size, char ** body);

/**
 * separate_req_body
 * <>
 * Separates a request body and allocates it into the appropriate request struct.
 * </p>
 * @param so the state object.
 * @return 0 on success, -1 on failure.
 */
static int separate_req_body(struct state_object * so);

/**
 * separate_req_CREATE
 * <p>
 * Separates CREATE requests into their appropriate struct.
 * </p>
 * @param so the state object.
 * @return 0 on success, -1 on failure.
 */
static int separate_req_CREATE(struct state_object * so);

/**
 * tokenize
 * <p>
 * Separates a request body into tokens.
 * </p>
 * @param raw_body the raw request body.
 * @param tokens character pointer array for tokens to be set in.
 * @param expected_tokens number of expected tokens in the body.
 * @return 0 on success, -1 on failure.
 */
static int tokenize(char * raw_body, char * tokens[], int expected_tokens);

/**
 * validate_req
 * <p>
 * Validates a request, making sure all data contained is valid.
 * </p>
 * @param so the state object.
 * @return 0 if valid, -1 if invalid.
 */
static int validate_req(struct state_object * so);

/**
 * marshal
 * <p>
 * Marshals any data not sent by the client into a request.
 * This data includes the sender IP.
 * </p>
 * @param so the state object.
 * @param fd the client socket file descriptor.
 * @return 0 on success, -1 on failure.
 */
static int marshal_req(int fd, struct state_object * so);

int read_request(int fd, struct state_object * so) {
    so->req = init_request();
    if (so->req == NULL) {
        so->req->result_code = INIT_REQUEST_ERROR;
        return READ_REQUEST_ERROR;
    }
    int read_header_result = read_header(fd, &so->req->header, &so->req->raw_header);
    if (read_header_result != READ_REQUEST_SUCCESS) {
        so->req->result_code = READ_HEADER_ERROR;
        return read_header_result;
    }
    if (read_body(fd, so->req->header.body_size, &so->req->raw_body) == -1) {
        so->req->result_code = ALLOCATE_ERROR;
        return READ_REQUEST_ERROR;
    }
    if (separate_req_body(so) == -1) {
        so->req->result_code = SEPARATE_BODY_ERROR;
        return READ_REQUEST_ERROR;
    }
    if (validate_req(so) == -1) {
        so->req->result_code = VALIDATE_BODY_ERROR;
        return READ_REQUEST_ERROR;
    }
    if (marshal_req(fd, so) == -1) {
        so->req->result_code = MARSHAL_ERROR;
        return READ_REQUEST_ERROR;
    }
    return READ_REQUEST_SUCCESS;
}

int read_header(int fd, struct header * h, uint32_t * raw_header) {
    int read_fully_result = read_fully(fd, raw_header, sizeof(*raw_header));
    if (read_fully_result == READ_FULLY_FAILURE) {
        return READ_REQUEST_ERROR;
    } else if (read_fully_result == READ_FULLY_EOF) {
        return READ_REQUEST_EOF;
    }
    *raw_header = ntohl(*raw_header);

    if (extract_version(*raw_header, h) == -1) {
        return READ_REQUEST_ERROR;
    }
    if (extract_action(*raw_header, h) == -1) {
        return READ_REQUEST_ERROR;
    }
    if (extract_object(*raw_header, h) == -1) {
        return READ_REQUEST_ERROR;
    }
    if (extract_body_size(*raw_header, h) == -1) {
        return READ_REQUEST_ERROR;
    }

    return READ_REQUEST_SUCCESS;
}

static int extract_version(uint32_t header, struct header * h) {
    uint32_t version = read_bits(header, 4, 0);
    if (version > 15 || version < 0) {
        (void) fprintf(stderr, "extracting header version: unexpected value %"PRIu32"\n", version);
        return -1;
    }
    h->version = (int)version;
    return 0;
}

static int extract_action(uint32_t header, struct header * h) {
    uint32_t action = read_bits(header, 4, 4);
    if (action > 15 || action < 0) {
        (void) fprintf(stderr, "extracting header action: unexpected value %"PRIu32"\n", action);
        return -1;
    }
    h->action = (enum action)action; // TODO support POLL and subtypes
    return 0;
}

static int extract_object(uint32_t header, struct header * h) {
    uint32_t object = read_bits(header, 8, 8);
    if (object > 255 || object < 0) {
        (void) fprintf(stderr, "extracting header object: unexpected value %"PRIu32"\n", object);
        return -1;
    }
    h->object = (enum object)object;
    return 0;
}

static int extract_body_size(uint32_t header, struct header * h) {
    uint16_t body_size = read_bits(header, 16, 16);
    if (body_size > 65535 || body_size < 0) {
        (void) fprintf(stderr, "extracting header object: unexpected value %"PRIu32"\n", body_size);
        return -1;
    }
    h->body_size = body_size;
    return 0;
}

static int read_body(int fd, uint32_t body_size, char ** body) {
    *body = calloc(body_size, sizeof(char));
    if (*body == NULL) {
        perror("calloc message body");
        return -1;
    }
    return read_fully(fd, *body, body_size);
}

static int separate_req_body(struct state_object * so) {
    // TODO create separate functions for READ, UPDATE, and DELETE
    switch (so->req->header.action) {
        case CREATE:
            return separate_req_CREATE(so);
        case READ:
            return 0;
        case UPDATE:
            return 0;
        case DELETE:
            return 0;
        default:
            (void) fprintf(stderr, "separate request body: unknown action\n");
            return -1;
    }
    return 0;
}

static int  separate_req_CREATE(struct state_object * so) {
    switch (so->req->header.object) {
        case USER:
        {
            char * tokens[3];
            if (tokenize(so->req->raw_body, tokens, 3) == -1) {
                return -1;
            }
            so->req->create_user = init_create_user_req(tokens[0], tokens[1], tokens[2]);
            if (so->req->create_user == NULL) {
                return -1;
            }
            break;
        }
        case CHANNEL:
        {
            char * tokens[3];
            if (tokenize(so->req->raw_body, tokens, 3) == -1) {
                return -1;
            }
            bool public = (strcmp(tokens[2], "1") == 0);
            so->req->create_channel = init_create_channel_req(tokens[0], tokens[1], public);
            if (so->req->create_channel == NULL) {
                return -1;
            }
            break;
        }
        case MESSAGE:
        {
            char * tokens[4];
            if (tokenize(so->req->raw_body, tokens, 4) == -1) {
                return -1;
            }
            uint64_t timestamp = strtoll(tokens[3], NULL, 16);
            if (timestamp == 0) {
                perror("converting timestamp to uint64");
                return -1;
            }
            so->req->create_message = init_create_message_req(tokens[0], tokens[1], tokens[2], timestamp);
            if (so->req->create_message == NULL) {
                return -1;
            }
            break;
        }
        case AUTH:
        {
            char * tokens[2];
            if (tokenize(so->req->raw_body, tokens, 2) == -1) {
                return -1;
            }
            so->req->create_auth = init_create_auth_req(tokens[0], tokens[1]);
            if (so->req->create_auth == NULL) {
                return -1;
            }
        }
        default:
            (void) fprintf(stderr, "separate CREATE request body: unknown object\n");
            return -1;
    }
    return 0;
}

static int tokenize(char * raw_body, char * tokens[], int expected_tokens) {
    char * token;
    char delim = DELIM;
    int i;

    token = strtok(raw_body, &delim);
    i = 0;
    while(token != NULL) {
        if (i >= expected_tokens) {
            (void) fprintf(stderr, "unexpected number of tokens in body: expected %d, got > %d\n", expected_tokens, i+1);
            return -1;
        }
        tokens[i] = token;
        token = strtok(NULL, &delim);
        i++;
    }
    if (i < expected_tokens) {
        (void) fprintf(stderr, "unexpected number of tokens in body: expected %d, got %d\n", expected_tokens, i);
        return -1;
    }

    return 0;
}

static int validate_req(struct state_object * so) {
    // TODO empty for now, separate_req_body does most error checking
    return 0;
}

static int marshal_req(int fd, struct state_object * so) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    if (getpeername(fd, (struct sockaddr *)&addr, &addr_size) == -1) {
        perror("reading sender address");
        return -1;
    }
    so->req->sender_ip = malloc(strlen(inet_ntoa(addr.sin_addr)) + 1);
    if (so->req->sender_ip == NULL) {
        perror("malloc sender IP");
        return -1;
    }
    strcpy(so->req->sender_ip, inet_ntoa(addr.sin_addr));
    return 0;
}

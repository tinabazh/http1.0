#include <response.h>

#include <core-lib/util.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DELIM "\x03"

// STANDARD RESPONSES //

int rawify_standard_response(int code, char ** body_dst) {
    size_t raw_body_len = 0;
    char response_code_str[4];
    const char* delim = DELIM;

    raw_body_len += 3 + 1; // error code and delim

    *body_dst = malloc(raw_body_len + 1); // extra space for \0, trimmed on writing
    if (*body_dst == NULL) {
        perror("malloc raw standard error response body");
        return -1;
    }
    (*body_dst)[0] = '\0';

    sprintf(response_code_str, "%d", code);
    strcat(*body_dst, response_code_str);
    strcat(*body_dst, delim);

    return 0;
}

struct standard_error_res * init_standard_error_res(char * error_message) {
    struct standard_error_res * se_res = malloc(sizeof(struct standard_error_res));
    if (se_res == NULL) {
        perror("malloc standard error response");
        return NULL;
    }
    memset(se_res, 0, sizeof(struct standard_error_res));

    se_res->error_message = strdup(error_message);
    if (se_res->error_message == NULL) {
        perror("malloc standard error response error message");
        return NULL;
    }

    return se_res;
}

int rawify_standard_error_res(int code, char ** body_dst, struct standard_error_res * sa_res) {
    size_t raw_body_len = 0;
    char response_code_str[4];
    const char *delim = "\3";

    raw_body_len += 3 + 1; // error code and delim
    raw_body_len += strlen(sa_res->error_message) + 1; // error message and delim

    *body_dst = malloc(raw_body_len + 1); // extra space for \0, trimmed on writing
    if (*body_dst == NULL) {
        perror("malloc raw standard error response body");
        return -1;
    }
    (*body_dst)[0] = '\0';

    sprintf(response_code_str, "%d", code);
    strcat(*body_dst, response_code_str);
    strcat(*body_dst, delim);

    strcat(*body_dst, sa_res->error_message);
    strcat(*body_dst, delim);

    return 0;
}

void destroy_standard_error_res(struct standard_error_res ** se_res) {
    if (*se_res != NULL) {
        if ((*se_res)->error_message != NULL) {
            free((*se_res)->error_message);
        }
        free(*se_res);
    }
    *se_res = NULL;
}

// CREATE RESPONSES //

struct create_auth_res * init_create_auth_res(char * display_name, int privilege_level, int channel_name_size, char ** channel_name_list) {
    struct create_auth_res * ca_res = malloc(sizeof(struct create_auth_res));
    if (ca_res == NULL) {
        perror("malloc create auth response");
        return NULL;
    }
    memset(ca_res, 0, sizeof(struct create_auth_res));

    ca_res->display_name = strdup(display_name);
    if (ca_res->display_name == NULL) {
        perror("malloc create auth response display name");
        return NULL;
    }
    ca_res->privilege_level = privilege_level;
    ca_res->channel_name_size = channel_name_size;
    ca_res->channel_name_list = channel_name_list;

    return ca_res;
}

int rawify_create_auth_res(int code, char ** body_dst, struct create_auth_res * ca_res) {
    size_t raw_body_len = 0;
    char response_code_str[4];
    char privilege_level_str[2];
    char channel_name_size_str[30];
    const char* delim = DELIM;

    raw_body_len += 3 + 1; // response code and delim
    raw_body_len += strlen(ca_res->display_name) + 1; // display name and delim
    raw_body_len += 1 + 1; // privilege level and delim
    raw_body_len += num_digits(ca_res->channel_name_size) + 1; // channel list size and delim
    for (int i = 0; i < ca_res->channel_name_size; i++) {
        raw_body_len += strlen(ca_res->channel_name_list[i]) + 1; // channel name and delim
    }

    *body_dst = malloc(raw_body_len + 1); // extra space for \0, trimmed on writing
    if (*body_dst == NULL) {
        perror("malloc raw create auth response body");
        return -1;
    }
    (*body_dst)[0] = '\0';

    sprintf(privilege_level_str, "%d", code);
    strcat(*body_dst, response_code_str);
    strcat(*body_dst, delim);

    strcat(*body_dst, ca_res->display_name);
    strcat(*body_dst, delim);

    sprintf(privilege_level_str, "%d", ca_res->privilege_level);
    strcat(*body_dst, privilege_level_str);
    strcat(*body_dst, delim);

    sprintf(privilege_level_str, "%d", ca_res->channel_name_size);
    strcat(*body_dst, channel_name_size_str);
    strcat(*body_dst, delim);

    for (int i = 0; i < ca_res->channel_name_size; i++) {
        strcat(*body_dst, ca_res->channel_name_list[i]);
        strcat(*body_dst, delim);
    }

    return 0;
}

void destroy_create_auth_res(struct create_auth_res ** ca_res) {
    if (*ca_res != NULL) {
        if ((*ca_res)->display_name != NULL) {
            free((*ca_res)->display_name);
        }
        if ((*ca_res)->channel_name_list != NULL) {
            for (int i = 0; i < (*ca_res)->channel_name_size; i++)
            {
                free((*ca_res)->channel_name_list[i]);
            }
            free((*ca_res)->channel_name_list);
        }
        free(*ca_res);
    }
    *ca_res = NULL;
}

struct create_user_non_unique_error_res * init_create_user_non_unique_error_res(int field_specifier, char * error_message) {
    struct create_user_non_unique_error_res * cunue_res = malloc(sizeof(struct create_user_non_unique_error_res));
    if (cunue_res == NULL) {
        perror("malloc create user non unique error response");
        return NULL;
    }
    memset(cunue_res, 0, sizeof(struct create_user_non_unique_error_res));

    cunue_res->error_message = strdup(error_message);
    if (cunue_res->error_message == NULL) {
        perror("malloc create user non unique error response error message");
        return NULL;
    }
    cunue_res->field_specifier = field_specifier;

    return cunue_res;
}

int rawify_create_user_non_unique_error_res(int code, char ** body_dst, struct create_user_non_unique_error_res * cunue_res) {
    size_t raw_body_len = 0;
    char response_code_str[4];
    char field_specifier_str[2];
    const char* delim = DELIM;

    raw_body_len += 3 + 1; // error code and delim
    raw_body_len += 1 + 1; // field specifier and delim
    raw_body_len += strlen(cunue_res->error_message) + 1; // error message and delim

    *body_dst = malloc(raw_body_len + 1); // extra space for \0, trimmed on writing
    if (*body_dst == NULL) {
        perror("malloc raw standard error response body");
        return -1;
    }
    (*body_dst)[0] = '\0'; // without it you hold garbage in the allocated mmry that doesn't allow strcat to find '\0' and properly concatenate it.


    sprintf(response_code_str, "%d", code);
    strcat(*body_dst, response_code_str);
    strcat(*body_dst, delim);

    sprintf(field_specifier_str, "%d", cunue_res->field_specifier);
    strcat(*body_dst, field_specifier_str);
    strcat(*body_dst, delim);

    strcat(*body_dst, cunue_res->error_message);
    // I make a note here if you come up with a question I changed it
    // strcat accepts two strings. You made delim only as a character that doesn't give '\0' for which strcat looking at
    // in it's loop. I had to make delim as a char * and to avoid warkings made it const.
    strcat(*body_dst, delim);

    return 0;
}

void destroy_create_user_non_unique_error_res(struct create_user_non_unique_error_res ** cunue_res) {
    if (*cunue_res != NULL) {
        if ((*cunue_res)->error_message != NULL) {
            free((*cunue_res)->error_message);
        }
        free(*cunue_res);
    }
    *cunue_res = NULL;
}

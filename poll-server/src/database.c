#include <database.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define USER_DB_FILE "user.db"
#define MESSAGE_DB_FILE "message.db"
#define CHANNEL_DB_FILE "channel.db"
#define AUTH_DB_FILE "auth.db"
#define UINT64_STR_BUF_SIZE 21

int init_db(struct core_object * co) {
    // TODO create the db if it doesn't exist, read if it does
    co->so->user_db = dbm_open(USER_DB_FILE, O_CREAT|O_RDWR, 0644);
    if(!co->so->user_db){
        fprintf(stderr, "Error: Unable to open user database.\n");
        return -1;
    }
    co->so->message_db = dbm_open(MESSAGE_DB_FILE, O_CREAT|O_RDWR, 0644);
    if(!co->so->message_db){
        fprintf(stderr, "Error: Unable to open message database.\n");
        return -1;
    }
    co->so->channel_db = dbm_open(CHANNEL_DB_FILE, O_CREAT|O_RDWR, 0644);
    if(!co->so->channel_db){
        fprintf(stderr, "Error: Unable to open channel database.\n");
        return -1;
    }
    co->so->auth_db = dbm_open(AUTH_DB_FILE, O_CREAT|O_RDWR, 0644);
    if(!co->so->auth_db){
        fprintf(stderr, "Error: Unable to open auth database.\n");
        return -1;
    }
    return 0;
}

void close_db(struct core_object * co) {
    dbm_close(co->so->user_db);
    dbm_close(co->so->message_db);
    dbm_close(co->so->auth_db);
    dbm_close(co->so->channel_db);
}

/**
 * Concatenate data
 * @param arr
 * @param delim
 * @param size
 * @return
 */
char *concat_with_delim(const char* arr[], const char *delim, int size) {
    unsigned long size_of_arr = 0;
    unsigned long size_of_delim = 0;
    if (size <= 0){
        return NULL;
    }
    for (int i = 0; i < size; ++i){
        size_of_arr += strlen(arr[i]);
        size_of_delim += strlen(delim);
    }
    char *result = malloc(size_of_arr + size_of_delim - strlen(delim) + 1);
    if (!result) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }
    strcpy(result, arr[0]);
    for (int i = 1; i < size; ++i){
        strcat(result, delim);
        strcat(result, arr[i]);
    }
    return result;
}

static int store_unique(DBM *db, const char *key_str, char *value_str, struct request *req) {
    datum key, data, existing;

    // create datum objects for the key and data
    key.dptr = key_str;
    key.dsize = strlen(key_str) + 1; // add 1 for null terminator
    data.dptr = value_str;
    data.dsize = strlen(value_str) + 1; // add 1 for null terminator

    // check if the key already exists
    existing = dbm_fetch(db, key);
    if (existing.dptr != NULL) {
        printf("Key '%s' already exists in database, value will not be updated\n", key_str);
        req->result_code = REQUEST_RESULT_CONFLICT;
        return -1;
    }

    // store the key-value pair in the database
    if (dbm_store(db, key, data, DBM_REPLACE) != 0) {
        fprintf(stderr, "Failed to store data in database\n");
        req->result_code = REQUEST_RESULT_INT_SERV_ERR;
        return -1;
    }

    printf("Value stored successfully\n");
    req->result_code = REQUEST_RESULT_CREATED; // Not sure if we should set result_code here
    return 0;
}

void create_user(struct core_object * co) {
    char *display_name = co->so->req->create_user->display_name;
    char *login_token = co->so->req->create_user->login_token;
    char *password = co->so->req->create_user->password;
    const char *delim = "\3";
    const char * arr[] = {display_name, password};
    char * data_cat = concat_with_delim(arr, delim, sizeof(arr)/sizeof(arr[0]));
    if(data_cat == NULL){
        // assign internal server error to result code
        co->so->req->result_code = REQUEST_RESULT_INT_SERV_ERR;
        return;
    }

    // store the value only if the key is unique
    int result = store_unique(co->so->user_db, login_token, data_cat, co->so->req);
    free(data_cat);
    if (result == -1){
        fprintf(stderr, "error in creating a user\n");
    } else {
        printf("User created\n");
    }
}

void create_channel(struct core_object * co, const char* uuid) {
    char * channel_name = co->so->req->create_channel->channel_name;
    char * display_name = co->so->req->create_channel->display_name;
    bool public = co->so->req->create_channel->public;
    char public_str[2];
    public_str[1] = '\0';
    public_str[0] = '0' + (char)public;
    const char * arr[] = {channel_name, display_name, public_str};
    const char *delim = "\3";
    char * data_cat = concat_with_delim(arr, delim, sizeof(arr)/sizeof(arr[0]));
    if(data_cat == NULL){
        // assign internal server error to result code
        co->so->req->result_code = REQUEST_RESULT_INT_SERV_ERR;
        return;
    }
    int result = store_unique(co->so->channel_db, uuid, data_cat, co->so->req);
    free(data_cat);
    if (result == -1){
        fprintf(stderr, "error in creating a channel\n");
    } else {
        printf("Channel created\n");
    }
}

int create_message(struct core_object * co, const char* uuid) {
    char *display_name = co->so->req->create_message->display_name;
    char *message = co->so->req->create_message->message_content;
    uint64_t timestamp = co->so->req->create_message->timestamp;
    char *channel_name = co->so->req->create_message->channel_name;
    const char *delim = "\3";

    // Declare a buffer on the stack with a size equal to the maximum length
    char num_str[UINT64_STR_BUF_SIZE];

    // Convert the uint64_t to a string representation and store it in the buffer
    snprintf(num_str, UINT64_STR_BUF_SIZE, "%lu", timestamp);
    const char *arr[] = {channel_name, display_name, num_str, message};
    char *data_cat = concat_with_delim(arr, delim, sizeof(arr)/ sizeof(arr[0]));
    if(data_cat == NULL){
        // assign internal server error to result code
        co->so->req->result_code = REQUEST_RESULT_INT_SERV_ERR;
        return -1;
    }
    // store the value only if the key is unique
    int result = store_unique(co->so->message_db, uuid, data_cat, co->so->req);
    free(data_cat);
    if (result == -1){
        fprintf(stderr, "error in creating a message\n");
    } else {
        printf("Message created\n");
    }
    return result;
}



int create_auth(struct core_object * co) {
    // TODO this one is a bit more complicated than the rest, reach out to Markus :)

    // TODO create an auth struct in objects.h, use here as a function parameter
    // TODO this should handle a user login

    char* password = co->so->req->create_auth->password;
    char * login_token = co->so->req->create_auth->login_token;
}

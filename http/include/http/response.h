#ifndef HTTPSERVER_RESPONSE_H
#define HTTPSERVER_RESPONSE_H

#include <unistd.h>
#include <stdbool.h>

enum res_result_code{
    RESPONSE_RESULT_SUCCESS = 200,
    RESPONSE_RESULT_CREATED = 201,
    RESPONSE_RESULT_BAD_REQUEST = 400,
    RESPONSE_RESULT_NOT_FOUND = 404,
    RESPONSE_RESULT_INVALID = 405,
    RESPONSE_RESULT_CONFLICT = 409, // not defined in the protocol
    RESPONSE_RESULT_INT_SERV_ERR = 500,
    RESPONSE_RESULT_CANNOT_HANDLE = 503,
    RESPONSE_RESULT_TIMEOUT = 504,
    RESPONSE_RESULT_WRONG_VERSION = 505
};

bool write_status_line(enum res_result_code res_code, int fd);

// return -1 in case of error
bool serve_file(const char* file_name, int fd);
#endif //HTTPSERVER_RESPONSE_H

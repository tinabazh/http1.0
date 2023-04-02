#include "response.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

/**
 * Return status message
 * @param res_code as enum
 * @return status message as string
 */
static const char *get_status_message(enum res_result_code res_code) {
    switch (res_code) {
        case RESPONSE_RESULT_SUCCESS: return "OK";
        case RESPONSE_RESULT_BAD_REQUEST: return "Bad Request";
        case RESPONSE_RESULT_NOT_FOUND: return "Not Found";
        case RESPONSE_RESULT_INT_SERV_ERR: return "Internal Server Error";
        default: return "Unknown";
    }
}

bool write_status_line(enum res_result_code res_code, int fd) {
    int length = dprintf(fd, "HTTP/1.1 %d %s\r\n", res_code, get_status_message(res_code));
    return length >= 0;
}

// return false in case of error
bool serve_file(const char* file_name, int fd) {
    if (!file_name){
        return false;
    }

    enum res_result_code res_code = RESPONSE_RESULT_SUCCESS;

    // Open the file which is either HTML, CSS, JS
    int file_fd = open(&file_name[1], O_RDONLY);
    struct stat file_stat;
    if (file_fd < 0) {
        // TODO: there could be other reasons for the error except file not existing
        res_code = RESPONSE_RESULT_NOT_FOUND;
    } else {
        // Obtain the file size
        if (fstat(file_fd, &file_stat) < 0) {
            close(file_fd);
            file_fd = -1;
            res_code = RESPONSE_RESULT_INT_SERV_ERR;
        }
    }

    // We cannot send anything if it fails
    if (write_status_line(res_code, fd) == false){
        return false;
    }

    if (file_fd >= 0) {
        size_t file_size = file_stat.st_size;
        // Write the response status line and headers
        int headers_length = dprintf(fd,
                                     "Content-Length: %zu\r\n"
                                     "\r\n",
                                     file_size);

        if (headers_length < 0) {
            close(file_fd);
            return false;
        }
        // Send the file in chunks
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
            if (write(fd, buffer, bytes_read) != bytes_read) {
                close(file_fd);
                return false;
            }
        }
        close(file_fd);
        return true;
    } else {
        if(dprintf(fd, "\r\n") < 0){
            return false;
        } else {
            return true;
        }
    }
}

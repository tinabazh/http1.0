#include <receiver.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <util.h>

void receiver_init(struct receiver * this, int fd) {
    this->fd = fd;
    this->start = 0;
    this->end = 0;
}

enum read_fully_result receiver_read(struct receiver * this, void * data, uint32_t size) {
    if (size == 0) {
        return READ_FULLY_SUCCESS;
    }

    // First, deliver preexisting data
    // Receiver is not a circular buffer. It must deliver all preexisting data before resetting
    // Thus, start is always >= end
    uint32_t deliver = this->end - this->start;
    if (size < deliver) {
        deliver = size;
    }
    if (deliver) {
        memcpy(data, &this->buffer[this->start], deliver);
        this->start += deliver;
        size -= deliver;
        if (size == 0) {
            return READ_FULLY_SUCCESS;
        }
        data += deliver;

        // At this point start == end, so reset the buffer
        this->start = 0;
        this->end = 0;
    }

    if (size < RECEIVER_BUFFER_LENGTH) {
        do {
            ssize_t result = recv(this->fd, this->buffer, RECEIVER_BUFFER_LENGTH, MSG_NOSIGNAL);
            if (result == -1) {
                perror("reading fully");
                return READ_FULLY_FAILURE;
            }
            if (result == 0) {
                // If it came to this, there was not enough data in the socket
                return READ_FULLY_EOF;
            }
            if (size < result) {
                this->start = size;
                this->end = result;
                result = size;
            }
            memcpy(data, this->buffer, result);
            data += result;
            size -= result;
        } while (size > 0);
        return READ_FULLY_SUCCESS;
    } else {
        return read_fully(this->fd, data, size);
    }
}

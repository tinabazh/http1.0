#include <receiver.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <util.h>
#include <stdbool.h>

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
    // Thus, start is always <= end
    uint32_t deliver = this->end - this->start;// located in the buffer, can be fulled partly or fully
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
                perror("receiver_read");
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

// returns whether the delimiter was successfully reached
static bool receiver_deliver_until(struct receiver * this, void * data, uint32_t* ret_size, char delimiter) {
    uint32_t ret_buf_size = *ret_size;
    uint32_t max_ind = this->end - this->start;
    if (max_ind > ret_buf_size) {
        max_ind = ret_buf_size;
    }
    max_ind += this->start;
    // First deliver preexisting data, if any
    for (uint32_t ind = this->start; ind < max_ind; ++ind) {
        if (this->buffer[ind] != delimiter) {
            *(char*)(data++) = this->buffer[ind];
        } else {
            *ret_size = ind - this->start;
            this->start = ind;
            return true;
        }
    }
    if (ret_buf_size < this->end - this->start) { // <data> buffer was not enough
        // ret_size remains unchanged as the whole buffer was used
        this->start = max_ind;
        // Check for an edge case. There's a chance the delimiter is in preexisting data
        return this->buffer[max_ind] == delimiter;
    }
    // else if (ret_buf_size == this->end - this->start) {
    // Still can check what goes after, but it's not in preexisting data. Need to read more from the socket.
    // So doing nothing here.
    // }
    // Not reached delimiter yet, resetting the buffer
    *ret_size =  this->end - this->start;
    this->start = this->end = 0;

    return false;
}


enum read_fully_result receiver_read_until(struct receiver * this, void * data, uint32_t* ret_size, char delimiter) {
    uint32_t ret_buf_size = *ret_size;
    bool reached; // reached the delimiter
    do {
        uint32_t delivered_size = ret_buf_size;
        reached = receiver_deliver_until(this, data, &delivered_size, delimiter);
        data += delivered_size;
        ret_buf_size -= delivered_size;
        if (reached) {
            *ret_size -= ret_buf_size;
            return READ_FULLY_SUCCESS;
        } else {
            // The buffer is empty at this point
            ssize_t result = recv(this->fd, this->buffer, RECEIVER_BUFFER_LENGTH, MSG_NOSIGNAL);
            if (result == -1) {
                perror("receiver_read_until");
                return READ_FULLY_FAILURE;
            }
            if (result == 0) {
                // If it came to this, there was not enough data in the socket
                return READ_FULLY_EOF;
            }
            this->end = result;
        }
    } while (!reached && ret_buf_size);

    *ret_size -= ret_buf_size;
    return READ_FULLY_SUCCESS;
}

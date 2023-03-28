#ifndef HTTPSERVER_RECEIVER_H
#define HTTPSERVER_RECEIVER_H

#include <stdint.h>
#include <core-lib/util.h>

#define RECEIVER_BUFFER_LENGTH 16

struct receiver {
    int fd;
    uint32_t start;
    uint32_t end;
    char buffer[RECEIVER_BUFFER_LENGTH];
};

void receiver_init(struct receiver *, int fd);

// Blocks until reads <size> bytes or until failure/eof is encountered
// Tries to read more than <size> bytes if available
enum read_fully_result receiver_read(struct receiver *, void * data, uint32_t size);

#endif //HTTPSERVER_RECEIVER_H

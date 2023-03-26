#include <util.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int write_fully(int fd, void * data, size_t size) {
    ssize_t result;
    ssize_t nwrote = 0;

    while (nwrote < (ssize_t)size) {
        result = send(fd, ((char*)data)+nwrote, size - nwrote, MSG_NOSIGNAL);
        if (result == -1) {
            perror("writing fully");
            return -1;
        }
        nwrote += result;
    }

    return 0;
}

int read_fully(int fd, void * data, size_t size) {
    if (size <= 0) {
        return READ_FULLY_SUCCESS;
    }
    ssize_t result;
    ssize_t nread = 0;

    do {
        result = read(fd, ((char*)data)+nread, size - nread);
        if (result == -1) {
            perror("reading fully");
            return READ_FULLY_FAILURE;
        }
        nread += result;
    } while (nread != 0 && nread < (ssize_t)size);

    return nread != 0 ? READ_FULLY_SUCCESS : READ_FULLY_EOF;
}

uint32_t read_bits(uint32_t bits, uint8_t num_bits, uint8_t start_at) {
    // network bit order is big endian=
    const uint8_t shift = 8 * sizeof(bits) - start_at - num_bits;
    uint32_t mask = (((uint32_t)1 << num_bits) - 1) << (shift);
    uint32_t res = bits & mask;
    res = res >> shift;
    return res;
}

void write_bits(uint32_t * bits, uint32_t value, uint32_t num_bits, uint32_t start_at) {
    uint32_t mask;
    int no_wrap = start_at > 0 ? 1 : 0; // prevents start_at - 1 from wrapping (black magic)

    mask = ~(((1 << num_bits) - 1) << (start_at - no_wrap));
    *bits = (*bits & mask) | (value << start_at);
}

int num_digits (int i) {
    if (i < 0) return num_digits ((i == INT_MIN) ? INT_MAX: -i);
    if (i < 10) return 1;
    return 1 + num_digits (i / 10);
}

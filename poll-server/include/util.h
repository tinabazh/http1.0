#ifndef POLL_SERVER_UTIL_H
#define POLL_SERVER_UTIL_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * write_fully
 * <p>
 * writes data fully to a file descriptor.
 * </p>
 * @param fd file descriptor to write to.
 * @param data data to write.
 * @param size size of data.
 * @return 0 on success. On failure -1 and set errno.
 */
int write_fully(int fd, void * data, size_t size);

enum read_fully_result{
    READ_FULLY_SUCCESS,
    READ_FULLY_FAILURE,
    READ_FULLY_EOF,
};

/**
 * read_fully
 * <p>
 * reads data fully from a file descriptor.
 * </p>
 * @param fd file descriptor to read from.
 * @param data where to write read data.
 * @param size size of data to read.
 * @return 0 on success. On failure -1 and set errno.
 */
int read_fully(int fd, void * data, size_t size);

/**
 * read_bits
 * <p>
 * read n bits from a starting position.
 * </p>
 * @param bits the bits to read from
 * @param num_bits number of bits to read
 * @param start_at position to start reading at
 * @return bits read
 * @example  read_bits(00111100000000000000000000000000, 4, 26) = 00000000000000000000000000001111
 */
uint32_t read_bits(uint32_t bits, uint8_t num_bits, uint8_t start_at);

/**
 * write_bits
 * <p>
 * write n bits from a starting position.
 * </p>
 * @param bits pointer to the bits to write to.
 * @param value bits being written.
 * @param num_bits number of bits to write.
 * @param start_at position to start writing at.
 * @example write_bits(00000000000000000000000000000000, 00000000000000000000000000001111, 4, 16) =
 * 00000000000011110000000000000000
 */
void write_bits(uint32_t * bits, uint32_t value, uint32_t num_bits, uint32_t start_at);

/**
 * num_digits
 * <p>
 * calculates and returns the number of digits in an integer.
 * </p>
 * @param i the integer.
 * @return the number of digits.
 */
int num_digits (int i);

#endif //POLL_SERVER_UTIL_H

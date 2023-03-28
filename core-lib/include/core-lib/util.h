#ifndef SCALABLE_SERVER_UTIL_H
#define SCALABLE_SERVER_UTIL_H

#include "api_functions.h"
#include "objects.h"

#include <sys/types.h>

#define DEFAULT_LIBRARY "../../one-to-one/cmake-build-debug/libone-to-one.dylib" // TODO: relative path should be changed to absolute.
#define DEFAULT_PORT "5000"
#define DEFAULT_IP "123.123.123.123" // TODO: will need to get the IP address by default

/**
 * api_functions
 * <p>
 * Struct containing pointers to all API functions.
 * </p>
 */
struct api_functions
{
    api initialize_server;
    api run_server;
    api close_server;
};

/**
 * trace_reporter
 * <p>
 * formatting function for trace reporting.
 * </p>
 * @param file_name name of the file the trace occurs in.
 * @param function_name name of the function the trace occurs in.
 * @param line_number the line the trace occurs in.
 */
void trace_reporter(const char *file_name, const char *function_name, size_t line_number);

/**
 * setup_core_object
 * <p>
 * Zero the core_object. Setup other objects and attach them to the core_object.
 * Open the log file and attach it to the core object.
 * </p>
 * @param co the core object
 * @param port_num the port number to listen on
 * @param ip_addr the ip address to listen on
 * @return 0 on success. On failure, -1 and set errno.
 */
int setup_core_object(struct core_object *co, in_port_t port_num,
                      const char *ip_addr);

/**
 * get_api
 * <p>
 * Open a given library and attempt to load API functions into the api_functions struct.
 * </p>
 * @param api struct containing API functions.
 * @param lib_name name of the library.
 * @return The opened library. NULL and set errno on failure.
 */
void *get_api(struct api_functions *api, const char *lib_name);

/**
 * close_lib
 * <p>
 * Close a dynamic library.
 * </p>
 * @param lib the library to close.
 * @param lib_name the name of the library to close
 * @return 0 on success. On failure, -1 and set errno.
 */
int close_lib(void *lib, const char *lib_name);

/**
 * destroy_core_object
 * <p>
 * Destroy the core object and all of its fields. Does not destroy the state object;
 * the state object must be destroyed by the library destroy_server function.
 * </p>
 * @param co the core object
 */
void destroy_core_object(struct core_object *co);


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
enum read_fully_result read_fully(int fd, void * data, size_t size);

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

#endif //SCALABLE_SERVER_UTIL_H

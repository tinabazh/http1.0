#include <core-lib/objects.h>
#include <core-lib/util.h>

#include <arpa/inet.h>
#include <dlfcn.h>
#include <mem_manager/manager.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>

#define LOG_FILE_NAME "log.csv"
#define LOG_OPEN_MODE "w" // Mode is set to truncate for independent results from each experiment.

#define API_INIT "initialize_server"
#define API_RUN "run_server"
#define API_CLOSE "close_server"

/**
 * open_file
 * <p>
 * Open a file with a given mode.
 * </p>
 * @param file_name the log file to open.
 * @param mode the mode to open the file with.
 * @return The file. NULL and set errno on failure.
 */
static FILE *open_file(const char * file_name, const char * mode);

/**
 * assemble_listen_addr
 * <p>
 * Assemble a the server's listen addr. Zero memory and fill fields.
 * </p>
 * @param listen_addr the address to assemble
 * @param port_num the port number
 * @param ip_addr the IP address
 * @param mm the memory manager object
 * @return 0 on success, -1 and set errno on failure.
 */
static int assemble_listen_addr(struct sockaddr_in *listen_addr, in_port_t port_num, const char *ip_addr);

/**
 * open_lib
 * <p>
 * Open a dynamic library in a given mode.
 * </p>
 * @param lib_name name of the library to open.
 * @param mode the mode to open the library with.
 * @return The library. NULL and set errno on failure.
 */
static void *open_lib(const char *lib_name, int mode);

/**
 * get_func
 * <p>
 * Get a function from a dynamic library. Function needs to be cast to the appropriate pointer.
 * </p>
 * @param lib the library to get from.
 * @param func_name the name of the function to get.
 * @return The function. NULL and set errno on failure.
 */
static void *get_func(void *lib, const char *func_name);

void trace_reporter(const char *file_name, const char *function_name, size_t line_number)
{
    (void) fprintf(stdout, "TRACE: %s : %s : @ %zu\n", file_name, function_name, line_number);
}

int setup_core_object(struct core_object *co, const in_port_t port_num,
                      const char *ip_addr)
{
    memset(co, 0, sizeof(struct core_object));

    co->mm  = init_mem_manager();
    if (!co->mm)
    {
        // NOLINTNEXTLINE(concurrency-mt-unsafe) : No threads here
        (void) fprintf(stderr, "Fatal: could not initialize memory manager: %s\n", strerror(errno));
        return -1;
    }
    co->log_file = open_file(LOG_FILE_NAME, LOG_OPEN_MODE);
    if (!co->log_file)
    {
        // NOLINTNEXTLINE(concurrency-mt-unsafe) : No threads here
        (void) fprintf(stderr, "Fatal: could not open %s: %s\n", LOG_FILE_NAME, strerror(errno));
        return -1;
    }
    
    if (assemble_listen_addr(&co->listen_addr, port_num, ip_addr) == -1)
    {
        // NOLINTNEXTLINE(concurrency-mt-unsafe) : No threads here
        (void) fprintf(stderr, "Fatal: could not assign server address: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

static FILE *open_file(const char *file_name, const char *mode)
{
    FILE *file;
    
    file = fopen(file_name, mode);
    // If an error occurs will return null.
    
    return file;
}

static int assemble_listen_addr(struct sockaddr_in *listen_addr, const in_port_t port_num, const char *ip_addr)
{
    int ret_val;
    
    memset(listen_addr, 0, sizeof(struct sockaddr_in));

    listen_addr->sin_port   = htons(port_num);
    listen_addr->sin_family = AF_INET;
    switch (inet_pton(AF_INET, ip_addr, &listen_addr->sin_addr.s_addr))
    {
        case 1: // Valid
        {
            ret_val = 0;
            break;
        }
        case 0: // Not a valid IP address
        {
            (void) fprintf(stderr, "%s is not a valid IP address\n", ip_addr);
            ret_val = -1;
            break;
        }
        default: // Some other error
        {
            ret_val = -1;
            break;
        }
    }
    
    return ret_val;
}

static void *open_lib(const char *lib_name, int mode)
{
    void *lib;
    
    lib = dlopen(lib_name, mode);
    // If an error occurs will return null.
    
    return lib;
}

int close_lib(void *lib, const char *lib_name)
{
    int status;
    
    status = dlclose(lib);
    if (status == -1)
    {
        // NOLINTNEXTLINE(concurrency-mt-unsafe) : No threads here
        (void) fprintf(stderr, "Fatal: could not close lib_name %s: %s\n", lib_name, strerror(errno));
    }
    
    return status;
    // If an error occurs will return -1.
}

void *get_api(struct api_functions *api, const char *lib_name)
{
    void *lib;
    bool get_func_err;
    
    // NOLINTBEGIN(concurrency-mt-unsafe) : No threads here
    lib = open_lib(lib_name, RTLD_LAZY);
    if (lib == NULL)
    {
        (void) fprintf(stderr, "Fatal: could not open API library %s: %s\n", lib_name, dlerror());
        return lib;
    }
    
    get_func_err = false;
    api->initialize_server = (int (*)(struct core_object *)) get_func(lib, API_INIT);
    if (api->initialize_server == NULL)
    {
        (void) fprintf(stderr, "Fatal: could not load API function %s: %s\n", API_INIT, strerror(errno));
        get_func_err = true;
    }
    api->run_server = (int (*)(struct core_object *)) get_func(lib, API_RUN);
    if (api->run_server == NULL)
    {
        (void) fprintf(stderr, "Fatal: could not load API function %s: %s\n", API_RUN, strerror(errno));
        get_func_err = true;
    }
    api->close_server = (int (*)(struct core_object *)) get_func(lib, API_CLOSE);
    if (api->close_server == NULL)
    {
        (void) fprintf(stderr, "Fatal: could not load API function %s: %s\n", API_CLOSE, strerror(errno));
        get_func_err = true;
    }
    // NOLINTEND(concurrency-mt-unsafe)
    
    if (get_func_err)
    {
        close_lib(lib, NULL);
        return NULL;
    }
    
    return lib;
}

static void *get_func(void *lib, const char *func_name)
{
    void *func;
    
    func = dlsym(lib, func_name);
    // If an error occurs will return null.
    
    return func;
}

void destroy_core_object(struct core_object *co)
{
    if (co->log_file)
    {
        (void) fclose(co->log_file);
    }
    free_mem_manager(co->mm);
}


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

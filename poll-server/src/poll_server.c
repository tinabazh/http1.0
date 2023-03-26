#include "../include/objects.h"
#include "../include/poll_server.h"

#include <read.h>
#include <handle.h>
#include <respond.h>
#include <broadcast.h>

#include <arpa/inet.h>
#include <errno.h>
#include <mem_manager/manager.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h> // back compatability
#include <sys/types.h>  // back compatability
#include <time.h>
#include <unistd.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables): must be non-const
/**
 * Whether the poll loop should be running.
 */
volatile int GOGO_POLL = 1;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

/**
 * execute_poll
 * <p>
 * Execute the poll function to listen for action of pollfds. Action on the
 * listen fd will call accept; otherwise, action will call read all.
 * </p>
 * @param co the core object
 * @param pollfds the pollfd array
 * @param nfds the number of fds in the array
 * @return 0 on success, -1 and set errno on failure
 */
static int execute_poll(struct core_object *co, struct pollfd *pollfds, nfds_t nfds);

/**
 * setup_signal_handler
 * @param sa sigaction struct to fill
 * @return 0 on success, -1 and set errno on failure
 */
static int setup_signal_handler(struct sigaction *sa, int signal);

/**
 * end_gogo_handler
 * <p>
 * Handler for signal. Set the running loop conditional to 0.
 * </p>
 * @param signal the signal received
 */
static void end_gogo_handler(int signal);

/**
 * poll_accept
 * <p>
 * Accept a new connection to the server. Set the value of the fd
 * increment the num connections in the state object and. Log the connection
 * in the log file.
 * </p>
 * @param co the core object
 * @param so the state object
 * @return the 0 on success, -1 and set errno on failure
 */
static int poll_accept(struct core_object *co, struct state_object *so, struct pollfd *pollfds);

/**
 * get_conn_index
 * <p>
 * Find an index in the file descriptor array where file descriptor == 0.
 * </p>
 * @param client_fds the file descriptor array
 * @return the first index where file descriptor == 0
 */
static int get_conn_index(const int *client_fds);

/**
 * poll_comm
 * <p>
 * Read from all file descriptors in pollfds for which POLLIN is set.
 * Remove all file descriptors in pollfds for which POLLHUP is set.
 * </p>
 * @param co the core object
 * @param so the state object
 * @param pollfds the pollfds array
 * @return 0 on success, -1 and set errno on failure
 */
static int poll_comm(struct core_object *co, struct state_object *so, struct pollfd *pollfds);

/**
 * reset_poll_state
 * <p>
 * Free the request and response from the state.
 * </p>
 * @param so the state object
 */
static void reset_poll_state(struct state_object * so);

/**
 * debug_print_req
 * <p>
 * Prints a request to the terminal.
 * </p>
 * @param so the state object.
 */
static void debug_print_req(struct state_object * so);

/**
 * debug_print_res
 * <p>
 * Prints a response to the terminal.
 * </p>
 * @param so the state object.
 */
static void debug_print_res(struct state_object * so);

/**
 * poll_remove_connection
 * <p>
 * Close a connection and remove the fd from the list of pollfds.
 * </p>
 * @param co the core object
 * @param so the state object
 * @param pollfd the pollfd to close and clean
 * @param conn_index the index of the connection in the array of client_fds and client_addrs
 * @param listen_pollfd the listen pollfd
 */
static void
poll_remove_connection(struct core_object *co, struct state_object *so, struct pollfd *pollfd, size_t conn_index,
                       struct pollfd *listen_pollfd);

/**
 * close_fd_report_undefined_error
 * <p>
 * Close a file descriptor and report an error which would make the file descriptor undefined.
 * </p>
 * @param fd the fd to close
 * @param err_msg the error message to print
 */
static void close_fd_report_undefined_error(int fd, const char *err_msg);

struct state_object *setup_poll_state(struct memory_manager *mm)
{
    struct state_object *so;
    
    so = (struct state_object *) Mmm_calloc(1, sizeof(struct state_object), mm);
    if (!so) // Depending on whether more is added to this state object, this if clause may go.
    {
        return NULL;
    }
    
    return so;
}

int open_poll_server_for_listen(struct core_object *co, struct state_object *so, struct sockaddr_in *listen_addr)
{
    int fd;
    
    fd = socket(PF_INET, SOCK_STREAM, 0); // NOLINT(android-cloexec-socket): SOCK_CLOEXEC dne
    if (fd == -1)
    {
        return -1;
    }
    
    if (bind(fd, (struct sockaddr *) listen_addr, sizeof(struct sockaddr_in)) == -1)
    {
        (void) close(fd);
        return -1;
    }
    
    if (listen(fd, CONNECTION_QUEUE) == -1)
    {
        (void) close(fd);
        return -1;
    }
    
    /* Only assign if absolute success. listen_fd == 0 can be used during teardown
     * to determine whether there is a socket to close. */
    so->listen_fd = fd;
    
    return 0;
}

int run_poll_server(struct core_object *co)
{
    struct pollfd pollfds[MAX_CONNECTIONS + 1]; // +1 for the listen socket.
    struct pollfd listen_pollfd;
    size_t        pollfds_len;
    
    // Set up the listen socket pollfd
    listen_pollfd.fd      = co->so->listen_fd;
    listen_pollfd.events  = POLLIN;
    listen_pollfd.revents = 0;
    pollfds_len = sizeof(pollfds) / sizeof(*pollfds);
    
    memset(pollfds, -1, sizeof(pollfds));
    
    pollfds[0] = listen_pollfd;
    
    // Set up the headers for the log file.
    (void) fprintf(co->log_file,
                   "connection index,file descriptor,ipv4 address,port number,bytes read,start timestamp,end timestamp,elapsed time (s)\n");
    
    if (execute_poll(co, pollfds, pollfds_len) == -1)
    {
        return -1;
    }
    
    return 0;
}

static int execute_poll(struct core_object *co, struct pollfd *pollfds, nfds_t nfds)
{
    int              poll_status;
    struct sigaction sigint;
    
    if (setup_signal_handler(&sigint, SIGINT) == -1)
    {
        return -1;
    }
    if (setup_signal_handler(&sigint, SIGTERM) == -1)
    {
        return -1;
    }
    
    while (GOGO_POLL)
    {
        poll_status = poll(pollfds, nfds, -1);
        if (poll_status == -1)
        {
            return (errno == EINTR) ? 0 : -1;
        }
        
        // If action on the listen socket.
        if ((*pollfds).revents == POLLIN)
        {
            if (poll_accept(co, co->so, pollfds) == -1)
            {
                return -1;
            }
        } else
        {
            printf("Accepted new connection\n");
            if (poll_comm(co, co->so, pollfds) == -1)
            {
                return -1;
            }
        }
    }
    
    return 0;
}

static int setup_signal_handler(struct sigaction *sa, int signal)
{
    sigemptyset(&sa->sa_mask);
    sa->sa_flags   = 0;
    sa->sa_handler = end_gogo_handler;
    if (sigaction(signal, sa, 0) == -1)
    {
        return -1;
    }
    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void end_gogo_handler(int signal)
{
    GOGO_POLL = 0;
}

#pragma GCC diagnostic pop

static int poll_accept(struct core_object *co, struct state_object *so, struct pollfd *pollfds)
{
    int       new_cfd;
    size_t    conn_index;
    socklen_t sockaddr_size;
    
    conn_index    = get_conn_index(so->client_fd);
    sockaddr_size = sizeof(struct sockaddr_in);
    
    new_cfd = accept(so->listen_fd, (struct sockaddr *) &so->client_addr[conn_index], &sockaddr_size);
    if (new_cfd == -1)
    {
        return -1;
    }
    
    so->client_fd[conn_index] = new_cfd; // Only save in array if valid.
    pollfds[conn_index + 1].fd     = new_cfd; // Plus one because listen_fd.
    pollfds[conn_index + 1].events = POLLIN;
    ++so->num_connections;
    
    if (so->num_connections >= MAX_CONNECTIONS)
    {
        pollfds->events = 0; // Turn off POLLIN on the listening socket when max connections reached.
    }
    
    // NOLINTNEXTLINE(concurrency-mt-unsafe): No threads here
    (void) fprintf(stdout, "Client connected from %s:%d\n", inet_ntoa(so->client_addr[conn_index].sin_addr),
                   ntohs(so->client_addr[conn_index].sin_port));
    
    return 0;
}

static int get_conn_index(const int *client_fds)
{
    int conn_index = 0;
    
    for (int i = 0; i < MAX_CONNECTIONS; ++i)
    {
        if (*(client_fds + i) == -1)
        {
            conn_index = i;
            break;
        }
    }
    return conn_index;
}

enum pollin_handle_result {
    POLLIN_HANDLE_RESULT_OK,
    POLLIN_HANDLE_RESULT_EOF,
    POLLIN_HANDLE_RESULT_FATAL,
};

static bool pollin_handle(struct core_object *co, struct state_object *so, struct pollfd *pollfd) {
    const int read_request_result = read_request(pollfd->fd, so);
    if (read_request_result == READ_REQUEST_SUCCESS) {
        handle_request(co);
    }
    if (read_request_result != READ_REQUEST_EOF) {
        // TODO remove this, only for demo purposes
        debug_print_req(so);

        if (write_response(pollfd->fd, so) == -1) {
            return POLLIN_HANDLE_RESULT_FATAL;
        }

        // TODO remove this, only for demo purposes
        debug_print_res(so);

        if (so->broadcast) { // TODO this isn't set by anything yet, should be set during handle_request()
            if (write_broadcast(co, so) == -1) {
                return POLLIN_HANDLE_RESULT_FATAL;
            }
        }
    }
    reset_poll_state(so);
    return read_request_result == READ_REQUEST_EOF ? POLLIN_HANDLE_RESULT_EOF : POLLIN_HANDLE_RESULT_OK;
}

static int poll_comm(struct core_object *co, struct state_object *so, struct pollfd *pollfds)
{
    struct pollfd *pollfd;
    
    for (size_t fd_num = 1; fd_num <= MAX_CONNECTIONS; ++fd_num)
    {
        pollfd = pollfds + fd_num;
        bool remove_connection = false;
        if (pollfd->revents == POLLIN)
        {
            const int pollin_result = pollin_handle(co, so, pollfd);
            if (pollin_result == POLLIN_HANDLE_RESULT_FATAL) {
                return -1;
            }
            remove_connection = pollin_result == POLLIN_HANDLE_RESULT_EOF;
        }
        if (remove_connection || (pollfd->revents & POLLHUP) || (pollfd->revents & POLLERR))
            // Client has closed other end of socket.
            // On MacOS, POLLHUP will be set; on Linux, POLLERR will be set.
        {
            (poll_remove_connection(co, so, pollfd, fd_num - 1, pollfds));
        }
        pollfd->revents = 0;
    }
    
    return 0;
}

static void reset_poll_state(struct state_object * so) {
    destroy_request(&so->req);
    destroy_response(&so->res);
    so->broadcast = false;
}

static void debug_print_req(struct state_object * so) {
    printf("HEADER:\nversion: %d\n action: %d\n object %d\nbody size: %d\n", so->req->header.version, so->req->header.action, so->req->header.object, so->req->header.body_size);
    if (so->req->create_user) {
        printf("CREATE USER:\ndisplay name: %s\nlogin token: %s\npassword: %s\n ", so->req->create_user->display_name, so->req->create_user->login_token, so->req->create_user->password);
    } else if (so->req->create_auth) {
        printf("CREATE AUTH:\nlogin token: %s\npassword: %s\n", so->req->create_auth->login_token, so->req->create_auth->password);
    } else if (so->req->create_message) {
        printf("CREATE MESSAGE:\ndisplay name: %s\nchannel name: %s\nmessage content: %s\ntimestamp: %"PRIu64"\n", so->req->create_message->display_name, so->req->create_message->channel_name, so->req->create_message->message_content, so->req->create_message->timestamp);
    } else if (so->req->create_channel) {
        printf("CREATE CHANNEL:\ndisplay name: %s\nchannel name: %s\npublic: %s\n", so->req->create_channel->display_name, so->req->create_channel->channel_name, so->req->create_channel->public ? "True" : "False");
    }
}

static void debug_print_res(struct state_object * so) {
    printf("RESPONSE RAW HEADER: %"PRIu32"\n", so->res->raw_header);
    printf("RESPONSE RAW BODY: %s\n", so->res->raw_body);
}

static void
poll_remove_connection(struct core_object *co, struct state_object *so, struct pollfd *pollfd, size_t conn_index,
                       struct pollfd *listen_pollfd)
{
    
    // close the fd
    close_fd_report_undefined_error(pollfd->fd, "state of client socket is undefined.");
    
    // NOLINTNEXTLINE(concurrency-mt-unsafe): No threads here
    (void) fprintf(stdout, "Client from %s:%d disconnected\n", inet_ntoa(so->client_addr[conn_index].sin_addr),
                   ntohs(so->client_addr[conn_index].sin_port));
    
    // zero the pollfd struct, the fd in the state object, and the client_addr in the state object.
    memset(pollfd, -1, sizeof(struct pollfd));
    memset(&so->client_addr[conn_index], 0, sizeof(struct sockaddr_in));
    so->client_fd[conn_index] = -1;
    --so->num_connections;
    
    if (listen_pollfd->events != POLLIN && so->num_connections < MAX_CONNECTIONS)
    {
        listen_pollfd->events = POLLIN; // Turn on POLLIN on the listening socket when less than max connections.
    }
}

void destroy_poll_state(struct core_object *co, struct state_object *so)
{

    reset_poll_state(so);

    close_fd_report_undefined_error(so->listen_fd, "state of listen socket is undefined.");
    
    for (size_t sfd_num = 0; sfd_num < MAX_CONNECTIONS; ++sfd_num)
    {
        close_fd_report_undefined_error(*(so->client_fd + sfd_num), "state of client socket is undefined.");
    }
}

static void close_fd_report_undefined_error(int fd, const char *err_msg)
{
    if (close(fd) == -1)
    {
        switch (errno)
        {
            case EBADF: // Not a problem.
            {
                errno = 0;
                break;
            }
            default:
            {
                // NOLINTNEXTLINE(concurrency-mt-unsafe) : No threads here
                (void) fprintf(stderr, "Error: %s; %s\n", strerror(errno), err_msg);
            }
        }
    }
}

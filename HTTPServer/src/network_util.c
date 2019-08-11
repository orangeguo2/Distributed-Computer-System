/*
 * network_util.c
 *
 * Functions that implement network operations.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

/**
 * Get listener socket
 *
 * @param port the port number
 * @return listener socket or 0 if unavailable
 */
int get_listener_socket(int port) {
    // Creating internet socket stream file descriptor
    int listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock_fd == 0) {
        return 0;
    }

    // SO_REUSEADDR prevents the "address already in use" errors
    // that commonly come up when testing servers.
    int optval = 1;
    if (setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval , sizeof(int)) < 0) {
    	close(listen_sock_fd);
    	return 0;
    }

    // host address and port
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    memset(&address, 0, addrlen);
    address.sin_family = AF_INET;  // address from internet
    address.sin_port = htons(port);   // port in network byte order
    address.sin_addr.s_addr = INADDR_ANY;  // bind to any address

    // bind host address to port
    if (bind(listen_sock_fd, (struct sockaddr *)&address, addrlen) < 0) {
    	close(listen_sock_fd);
		return 0;
    }

    // set up queue for clients connections up to default
    // maximum pending socket connections (usually 128)
    if (listen(listen_sock_fd, SOMAXCONN) < 0) {
    	close(listen_sock_fd);
    	return 0;
    }

	return listen_sock_fd;
}

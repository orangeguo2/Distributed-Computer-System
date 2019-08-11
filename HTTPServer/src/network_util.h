/*
 * network_util.h
 *
 * Functions that implement network operations.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

/**
 * Get listener socket
 *
 * @param port the port number
 * @return listener socket or 0 if unavailable
 */
int get_listener_socket(int port) ;

#endif /* NETWORK_UTIL_H_ */

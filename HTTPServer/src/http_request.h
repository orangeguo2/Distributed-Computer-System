/*
 * http_request.h
 *
 * Functions used to process requests from clients.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

/**
 *  Process an http request.
 *  @param sock_fd the socket descriptor
 */
void process_request(int sock_fd);


#endif /* HTTP_REQUEST_H_ */

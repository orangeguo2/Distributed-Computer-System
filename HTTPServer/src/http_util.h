/*
 * http_util.h
 *
 * Functions used to implement http operations.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#ifndef HTTP_UTIL_H_
#define HTTP_UTIL_H_

#include <time.h>

#include "properties.h"

/**
 * Reads request headers from request stream until empty line.
 *
 * @param istream the socket input stream
 * @param request headers
 * @throws IOException if error occurs
 */
void readRequestHeaders(FILE *istream, Properties *requestHeader);

/**
 * Send bytes for status to response output stream.
 *
 * @param status the response status
 * @param statusMsg the response message
 * @param responseCharset the response charset
 * @throws IOException if error occurs
 */
void sendResponseStatus(FILE *ostream, int status, const char *statusMsg);

/**
 * Copy bytes from input stream to output stream
 * @param istream the input stream
 * @param ostream the output stream
 * @param nbytes the number of bytes to send
 * @param return 0 if successful
 */
int sendResponseBytes(FILE *istream, FILE *ostream, int nbytes);

/**
 * Send bytes for headers to response output stream.
 *
 * @param responseHeaders the header name value pairs
 * @param responseCharset the response charset
 * @throws IOException if errors occur
 */
void sendResponseHeaders(FILE *ostream, Properties *responseHeaders);

/**
 * Set error response and error page to the response output stream.
 *
 * @param ostream the output socket stream
 * @param status the response status
 * @param statusMsg the response message
 * @param responseHeaders the response headers
 */
void sendErrorResponse(FILE* ostream, int responseCode, const char *responseStr, Properties *responseHeaders);

/**
 * Unescape a URI string by replacing %xx with
 * the corresponding character code.
 * @param escUrl the esc URI
 * @param uri the decoded URI
 * @return the URL if successful, NULL if error
 */
char *unescapeUri(const char *escUri, char *uri);

/**
 * Resolves server URI to file system path.
 * @param uri the request URI
 * @param fspath the file system path
 * @return the file system path
 */
char *resolveUri(const char *uri, char *fspath);

/**
 * Debug request by printing request and request headers
 *
 * @param request the request line
 * @param requestHeaders the request headers
 */
void debugRequest(const char *request, Properties *requestHeaders);

#endif /* HTTP_UTIL_H_ */

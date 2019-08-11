/*
 * http_request.c
 *
 * Functions used to process requests from clients.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "http_methods.h"
#include "http_util.h"
#include "time_util.h"
#include "http_server.h"


/**
 *  Process an http request.
 *  @param sock_fd the socket descriptor
 */
void process_request(int sock_fd) {
	char buf[MAXBUF];
	char request[MAXBUF];
	char method[MAXBUF];
	char uri[MAXBUF], encUri[MAXBUF];
	char version[MAXBUF];

	// open socket as a stream
	FILE *stream = fdopen(sock_fd, "r+");
	if (stream == NULL) {
		perror("fdopen");
		return;
	}

	// get header line
	if (fgets(request, MAXBUF, stream) == NULL) {
		return;
	}
	// eliminate newline
	char *p = strstr(request, CRLF);
	if (p != NULL) {
		*p = '\0';
	}

	// initialize request headers
	Properties *responseHeaders = newProperties();
	// name of server
	putProperty(responseHeaders, "Server", "Tiny C Http Server");

	// date and time of this response
	time_t timer;
	time(&timer); // need to get local file time?
	putProperty(responseHeaders,"Date",
				milliTimeToRFC_1123_Date_Time(timer, buf));


	// decode header
	if (sscanf(request, "%s %s %s", method, encUri, version) != 3) {
		if (debug) {
			fprintf(stderr, "request header incomplete: %s\n", request);
		}
		sendErrorResponse(stream, 400, "Bad Request", responseHeaders);
		return;
	}
	// initialize request headers
	Properties *requestHeaders = newProperties();
	readRequestHeaders(stream, requestHeaders);
	if (debug) {
		debugRequest(request, requestHeaders);
	}

	// save query parameters as key "?"
	p = strpbrk(encUri,"?&");
	if (p != NULL) {
		putProperty(requestHeaders, "?", p+1);
		*p = '\0';
		if (debug) {
			fprintf(stderr, "Query: %s\n", p+1);
		}
	}

	// unescape URI
	if (unescapeUri(encUri, uri) == NULL) {
		if (debug) {
			fprintf(stderr, "request header invalid URI encoding %s\n", request);
		}
		sendErrorResponse(stream, 400, "Bad Request", responseHeaders);
		return;
	}

	// dispatch based on method
	if (strcasecmp(method, "GET") == 0) {
		do_get(stream, uri, requestHeaders, responseHeaders);
	} else 	if (strcasecmp(method, "HEAD") == 0) {
		do_head(stream, uri, requestHeaders, responseHeaders);
	} else 	if (strcasecmp(method, "PUT") == 0) {
		do_put(stream, uri, requestHeaders, responseHeaders);
	} else 	if (strcasecmp(method, "POST") == 0) {
		do_post(stream, uri, requestHeaders, responseHeaders);
	} else 	if (strcasecmp(method, "PUT") == 0) {
		do_post(stream, uri, requestHeaders, responseHeaders);
	} else 	if (strcasecmp(method, "DELETE") == 0) {
		do_delete(stream, uri, requestHeaders, responseHeaders);
	} else {
		sendErrorResponse(stream, 501, "Not Implemented", responseHeaders);
	}

	// delete headers
	deleteProperties(requestHeaders);
	deleteProperties(responseHeaders);

	// close socket stream
	fflush(stream);
	fclose(stream);
	close(sock_fd);
}


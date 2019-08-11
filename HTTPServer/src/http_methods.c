/*
 * methods.c
 *
 * Functions that implement HTTP methods, including
 * GET, HEAD, PUT, POST, and DELETE.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#include "http_methods.h"

#include <stddef.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "http_server.h"
#include "http_util.h"
#include "time_util.h"
#include "mime_util.h"
#include "properties.h"
#include "file_util.h"

/**
 * Handle GET or HEAD request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 * @param sendContent send content (GET)
 */
static void do_get_or_head(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders, bool sendContent) {
	// get path to URI in file system
	char filePath[MAXBUF];
	resolveUri(uri, filePath);

	// ensure file exists
	struct stat sb;
	if (stat(filePath, &sb) != 0) {
		sendErrorResponse(stream, 404, "Not Found", responseHeaders);
		return;
	}
	// ensure file is a regular file
	if (!S_ISREG(sb.st_mode)) {
		sendErrorResponse(stream, 404, "Not Found", responseHeaders);
		return;
	}

	// record the file length
	char buf[MAXBUF];
	size_t contentLen = (size_t)sb.st_size;
	sprintf(buf,"%lu", contentLen);
	putProperty(responseHeaders,"Content-Length", buf);

	// record the last-modified date/time
	time_t timer = sb.st_mtim.tv_sec;
	putProperty(responseHeaders,"Last-Modified",
				milliTimeToRFC_1123_Date_Time(timer, buf));

	// get mime type of file
	getMimeType(filePath, buf);
	putProperty(responseHeaders, "Content-type", buf);

	// send response
	sendResponseStatus(stream, 200, "OK");

	// Send response headers
	sendResponseHeaders(stream, responseHeaders);

	if (sendContent) {  // for GET
		FILE *contentStream = fopen(filePath, "r");
		sendResponseBytes(contentStream, stream, contentLen);
		fclose(contentStream);
	}
}

/**
 * Handle GET request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 * @param headOnly only perform head operation
 */
void do_get(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	do_get_or_head(stream, uri, requestHeaders, responseHeaders, true);
}

/**
 * Handle HEAD request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_head(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	do_get_or_head(stream, uri, requestHeaders, responseHeaders, false);
}

/**
 * Handle PUT request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_put(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	sendErrorResponse(stream, 405, "Method Not Allowed", responseHeaders);
}

/**
 * Handle POST request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_post(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	sendErrorResponse(stream, 405, "Method Not Allowed", responseHeaders);
}

/**
 * Handle DELETE request.
 *
 * @param the socket stream
 * @param uri the request URI
 * @param requestHeaders the request headers
 * @param responseHeaders the response headers
 */
void do_delete(FILE *stream, const char *uri, Properties *requestHeaders, Properties *responseHeaders) {
	sendErrorResponse(stream, 405, "Method Not Allowed", responseHeaders);
}

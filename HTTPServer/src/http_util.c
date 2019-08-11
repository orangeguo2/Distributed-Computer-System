/*
 * http_util.c
 *
 * Functions used to implement http operations.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#include <stdio.h>
#include <string.h>
#include "properties.h"
#include "file_util.h"
#include "http_server.h"


/** The default response protocol */
static const char *responseProtocol = "HTTP/1.1";

/** The content base for the web server */
extern const char *CONTENT_BASE;


/**
 * Reads request headers from request stream until empty line.
 *
 * @param istream the socket input stream
 * @param request headers
 */
void readRequestHeaders(FILE *istream, Properties *requestHeaders) {
	char buf[MAXBUF];

	while (fgets(buf, MAXBUF, istream) != NULL) {
		// trim newline characters
		char *p = strstr(buf, CRLF);
		if (p != NULL) {
			*p = 0;
		}

		// empty line marks send of headers
		if (*buf == '\0') {
			break;
		}

		p = strchr(buf, ':'); // find name/value delimiter
		if (p != NULL) {
			for (*p++ = '\0'; *p == ' '; p++) {} // skip over leading value whitespace
			if (!putProperty(requestHeaders, buf, p)) {  // save request property
				fprintf(stderr, "readRequestHeaders requestHeaders full\n");
				break;
			}
		}
	}
}

/**
 * Send bytes for status to response output stream.
 *
 * @param ostream the output socket stream
 * @param status the response status
 * @param statusMsg the response message
 */
void sendResponseStatus(FILE *ostream, int status, const char *statusMsg) {
	fprintf(ostream, "%s %d %s %s", responseProtocol, status, statusMsg, CRLF);
	if (debug) {
		fprintf(stderr, "%s %d %s\n", responseProtocol, status, statusMsg);
	}
}

/**
 * Copy bytes from input stream to output stream
 * @param istream the input stream
 * @param ostream the output stream
 * @param nbytes the number of bytes to send
 * @param return 0 if successful
 */
int sendResponseBytes(FILE *istream, FILE *ostream, int nbytes) {
	char *buf[MAXBUF];
    while ((nbytes > 0) && !feof(istream)) {
    	int ntoread = (nbytes < MAXBUF) ? nbytes : MAXBUF;
        size_t nread = fread(buf, sizeof(char), ntoread, istream);
        if (nread > 0) {
			if (fwrite(buf, sizeof(char), nread, ostream) < nread) {
				perror("sendResponseBytes");
			}
			nbytes -= nread;
		}
    }
    return 0;
}

/**
 * Send bytes for headers to response output stream.
 *
 * @param responseHeaders the header name value pairs
 * @param responseCharset the response charset
 */
void sendResponseHeaders(FILE *ostream, Properties *responseHeaders) {
	// output headers
	char name[MAX_PROP_NAME], val[MAX_PROP_VAL];
	for (int i = 0; getProperty(responseHeaders, i, name, val); i++) {
		fprintf(ostream, "%s: %s%s", name, val, CRLF);
    	if (debug) {
    		fprintf(stderr, "%s: %s\n", name, val);
    	}
	}

	// Send a blank line to indicate the end of the header lines.
	fprintf(ostream, "%s", CRLF);
	if (debug) {
		fprintf(stderr, "\n");
	}
}

/**
 * Set error response and error page to the response output stream.
 *
 * @param ostream the output socket stream
 * @param status the response status
 * @param statusMsg the response message
 * @param responseHeaders the response headers
 */
void sendErrorResponse(FILE* ostream, int responseCode, const char *responseStr, Properties *responseHeaders) {
	sendResponseStatus(ostream, responseCode, responseStr);

	char errorBody[2*MAXBUF];  // because of data substitution.
	const char *errorPage =
		"<html>"
	    "<head><title>%d %s</title></head>"
	    "<body>%d %s"
	    "<br>usage:http://yourHostName:port/"
	    "fileName.html</body></html>";
	sprintf(errorBody, errorPage, responseCode, responseStr, responseCode, responseStr);
	FILE *tmpStream = tmpStringFile(errorBody);

	char buf[MAXBUF];
	size_t contentLen = strlen(errorBody);
	sprintf(buf, "%lu", contentLen);
	putProperty(responseHeaders,"Content-Length", buf);
	putProperty(responseHeaders,"Content-type", "text/html");


	// Send the headers
	sendResponseHeaders(ostream, responseHeaders);

	// Send the error page body.
	// send the content
	sendResponseBytes(tmpStream, ostream, contentLen);
	fclose(tmpStream);
}

/**
 * Unescape a URI string by replacing %xx with
 * the corresponding character code.
 * @param escUrl the esc URI
 * @param uri the decoded URI
 * @return the URL if successful, NULL if error
 */
char *unescapeUri(const char *escUri, char *uri) {
	char *p = uri;
	while (*escUri) {
		if ( *escUri == '%') {
			int c;
			if (sscanf(escUri, "%%%02x", &c) == 0) {
				return NULL;
			}
			*p++ = (unsigned char)c;
			escUri+=3;
		} else {
			*p++ = *escUri++;
		}
	}
	*p++ = '\0';
	return uri;
}

/**
 * Resolves server URI to file system path.
 * @param uri the request URI
 * @param fspath the file system path
 * @return the file system path
 */
char *resolveUri(const char *uri, char *fspath) {
	strcpy(fspath, CONTENT_BASE);
	strcat(fspath, uri);
	return fspath;
}

/**
 * Debug request by printing request and request headers
 *
 * @param request the request line
 * @param requestHeaders the request headers
 */
void debugRequest(const char *request, Properties *requestHeaders) {
	char name[MAXBUF], val[MAXBUF];
	fprintf(stderr, "\n%s\n", request);
	for (int i = 0; getProperty(requestHeaders, i, name, val); i++) {
		fprintf(stderr, "%s: %s\n", name, val);
	}
	fprintf(stderr, "\n");
}


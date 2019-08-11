/*
 * mime_util.c
 *
 * Functions for processing MIME types.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mime_util.h"
#include "http_server.h"

static const char *DEFAULT_MIME_TYPE = "application/octet-stream";

/**
 * Lowercase a string
 */
char *strlower(char *s)
{
    for (char *p = s; *p != '\0'; p++) {
        *p = tolower(*p);
    }

    return s;
}

/**
 * Return a MIME type for a given filename.
 *
 * @param filename the name of the file
 * @param mimeType output buffer for mime type
 * @return pointer to mime type string
 */
char *getMimeType(const char *filename, char *mimeType)
{
	// get the file extension
    char *ext = strrchr(filename, '.');

    if (ext == NULL) { // default if no extension
    	strcpy(mimeType, DEFAULT_MIME_TYPE);
    	return mimeType;
    }

    char buf[MAXBUF];
    strcpy(buf, ++ext); // copy and lower-case the extension
    strlower(ext);

    const char *mtstr;

    // hash on first char?
    switch (*ext) {
    case 'c':
        if (strcmp(ext, "css") == 0) { mtstr = "text/css"; }
        break;
    case 'g':
        if (strcmp(ext, "gif") == 0) { mtstr = "image/gif"; }
        break;
    case 'h':
        if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) { mtstr = "text/html"; }
        break;
    case 'j':
    	if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) { mtstr = "image/jpg"; }
    	else if (strcmp(ext, "js") == 0) { mtstr = "application/javascript"; }
    	else if (strcmp(ext, "json") == 0) { mtstr = "application/json"; }
    	break;
    case 'p':
        if (strcmp(ext, "png") == 0) { mtstr = "image/png"; }
        break;
    case 't':
    	if (strcmp(ext, "txt") == 0) { mtstr = "text/plain"; }
    	break;
    default:
    	mtstr = DEFAULT_MIME_TYPE;
    }

    strcpy(mimeType, mtstr);
    return mimeType;
}

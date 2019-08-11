/*
 * file_util.c
 *
 * Utilities functions for working with C FILE streams.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "file_util.h"

/**
 * This function creates a temporary stream for this string.
 * When the FILE is closed, it will be automatically removed.
 *
 * @param contentStr the content of the file
 * @return the FILE for this string
 */
FILE *tmpStringFile(const char *contentStr) {
	FILE *tmpstream = tmpfile();
	fwrite(contentStr, 1, strlen(contentStr), tmpstream);
	rewind(tmpstream);
	return tmpstream;
}

/**
 * This function calls fstat() on the file descriptor of the
 * specified stream.
 *
 * @param stream the stream
 * @param buf the stat struct
 * @return 0 if successful, -1 with errno set if error.
 */
int fileStat(FILE *stream, struct stat *buf) {
	int fd = fileno(stream);
	return fstat(fd, buf);
}

/**
 * Returns path component of the file path without trailing
 * path separator. If no path component, returns NULL.
 *
 * @param filePath the path and file
 * @param pathOfFile return buffer (must be large enough)
 * @return pointer to pathOfFile or NULL if no path
 */
char *getPath(const char *filePath, char *pathOfFile) {
	char *p = strrchr(filePath, '/');
	if (p == NULL) {
		return NULL;
	}
	strncpy(pathOfFile, filePath, p-filePath);
	pathOfFile[p-filePath] = '\0';  // must terminate;
	return pathOfFile;
}

/**
 * Make a file path by combining a path and a file name.
 * If the file name begins with '/', return the name as an
 * absolute. Otherwise, concatenate the path and name,
 * ensuring there is a '/' separator between them.
 *
 * @param path the path component
 * @param name the file name component
 * @return the file path
 */
char *makeFilePath(const char *path, const char *name, char *filepath) {
	if (name[0] == '/') {
		strcpy(filepath, name);
	} else {
		strcpy(filepath, path);
		if (path[strlen(path)-1] != '/') {
			strcat(filepath, "/");
		}
		strcat(filepath, name);
	}
	return filepath;
}

/**
 * Make directories specified by path.
 *
 * @param path the path to create
 * @param mode mode if a directory is created
 * @return 0 if successful, -1 if error
 */
int mkdirs(const char *path, mode_t mode) {
	char buf[strlen(path)+1];
	const char *p = (*path == '/') ? path+1 : path;
	while (*p != '\0') {
		p = strchr(p, '/');  // find next path separator
		if (p == NULL) {     // last path element
			p = path + strlen(path);
		}
		strncpy(buf, path, p-path);
		buf[p-path] = '\0';  // must terminate
		if (mkdir(buf, mode) == -1) {
			if (errno == ENOTDIR) {
				return -1;
			}
		}
		if (*p == '/') {  // advance past path separator
			p++;
		}
	}
	return 0;
}

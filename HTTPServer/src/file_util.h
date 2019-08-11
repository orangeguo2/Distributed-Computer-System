/*
 * file_util.h
 *
 * Utilities functions for working with C FILE streams.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include <stdio.h>
#include <sys/stat.h>

// MacOS uses non-standard name for stat time fields
#if defined(__MACH__) && defined(__APPLE__)
#define st_mtim st_mtimespec
#define st_ctim st_ctimespec
#define st_atim st_atimespec
#endif

/**
 * This function creates a temporary stream for this string.
 * When the FILE is closed, it will be automatically removed.
 *
 * @param contentStr the content of the file
 * @return the FILE for this string
 */
FILE *tmpStringFile(const char *contentStr);

/**
 * This function calls fstat() on the file descriptor of the
 * specified stream.
 *
 * @param stream the stream
 * @param buf the stat struct
 * @return 0 if successful, -1 with errno set if error.
 */
int fileStat(FILE *stream, struct stat *buf);

/**
 * Returns path component of the file path without trailing
 * path separator. If no path component, returns NULL.
 *
 * @param filePath the path and file
 * @param pathOfFile return buffer (must be large enough)
 * @return pointer to pathOfFile or NULL if no path
 */
char *getPath(const char *filePath, char *pathOfFile);

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
char *makeFilePath(const char *path, const char *name, char *filepath);

/**
 * Make directories specified by path.
 *
 * @param path the path to create
 * @param mode mode if a directory is created
 * @return 0 if successful, -1 if error
 */
int mkdirs(const char *path, mode_t mode);

#endif /* FILE_UTIL_H_ */

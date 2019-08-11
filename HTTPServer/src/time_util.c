/*
 * time_util.c
 *
 * Functions that implement time operations.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#include "time_util.h"

/**
 * Converts timer to a RFC-1123 formatted date-time string
 * of the form: Sat, 13 Apr 2019 19:03:32 GMT
 * @param timer the time
 * @param buf the buffer
 * @return pointer to the buffer
 */
char *milliTimeToRFC_1123_Date_Time(time_t timer, char *buf) {
	struct tm* tm_info = gmtime(&timer);
	strftime(buf, 128, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
	return buf;
}

/**
 * Converts timer to short formatted date-time string
 * of the form: 2015-11-18 08:43
 * of the form
 * @param timer the time
 * @param buf the buffer
 * @return pointer to the buffer
 */
char *milliTimeToShortHM_Date_Time(time_t timer, char *buf) {
	struct tm* tm_info = gmtime(&timer);
	strftime(buf, 128, "%F %H:%M", tm_info);
	return buf;
}

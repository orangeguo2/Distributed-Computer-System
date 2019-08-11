/*
 * properties.h
 *
 * Functions that implement simple property lists.
 *
 *  @since 2019-04-10
 *  @author: Philip Gust
 */

#ifndef PROPERTIES_H_
#define PROPERTIES_H_
#include <stdbool.h>
#include <stdint.h>

#define MAX_PROP_NAME 64
#define MAX_PROP_VAL 128

/** Declaration of Properties as opaque type */
typedef struct Properties Properties;

/**
 * Create a new properties.
 * @return a new properties
 */
Properties *newProperties();

/**
 * Delete a properties
 * @param a properties
 */
void deleteProperties(Properties *props);

/**
 * Put a property to the properties.
 * @param a properties
 * @param name a property name
 * @param val a property value
 * @return true if property added
 */
bool putProperty(Properties *props, const char *name, const char *val);

/**
 * Get name and value for the specified property index.
 * @param props a properties
 * @param propIndex the property index
 * @param name storage for the name
 * @param val storage for the value
 * @return true if property at specified index is available
 */
bool getProperty(Properties *props, size_t propIndex, char *name, char *val);

/**
 * Find a property by name, starting with specified property index.
 * @param props the properties
 * @param propIndex the starting property index
 * @param name prop name
 * @param val storage for the value
 * @return the index of the value found or SIZE_MAX if not found
 */
size_t findProperty(Properties *props, size_t propIndex, const char *name, char *val);

/**
 * Return number of properties.
 * @param props the properties
 * @return number of properties;
 */
size_t nProperties(const Properties *props);

#endif /* PROPERTIES_H_ */

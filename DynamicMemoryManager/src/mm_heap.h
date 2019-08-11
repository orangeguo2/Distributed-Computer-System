/*
 * mm_heap.h
 *
 *  @since 2019-02-27
 *  @author philip gust
 */

#ifndef MM_HEAP_H_
#define MM_HEAP_H_

/**
 * Initialize memory allocator
 */
void mm_init();

/**
 * Reset memory allocator
 */
void mm_reset();

/**
 * De-initialize memory allocator
 */
void mm_deinit();

/**
 * Calculate the total amount of available free memory.
 *
 * @return the amount of free memory in bytes
 */
size_t mm_getfree(void);


/**
 * Allocates size bytes of memory and returns a pointer to the
 * allocated memory, or NULL if request storage cannot be allocated.
 *
 * @param nbytes the number of bytes to allocate
 * @return pointer to allocated memory or NULL if not available.
 */
void *mm_malloc(size_t nbytes);

/**
 * Deallocates the memory allocation pointed to by ptr.
 * if ptr is a NULL pointer, no operation is performed.
 *
 * @param ap the allocated block to free
 */
void mm_free(void *ap);

/**
 * Reallocates size bytes of memory and returns a pointer to the
 * allocated memory, or NULL if request storage cannot be allocated.
 *
 * @param ap the currently allocated storage
 * @param nbytes the number of bytes to allocate
 * @return pointer to allocated memory or NULL if not available.
 */
void *mm_realloc(void *ap, size_t size);


#endif /* MM_HEAP_H_ */

/*
 * memlib.c - a module that simulates the memory system.  Needed because it
 *            allows us to interleave calls from the student's malloc package
 *            with the system's malloc package in libc.
 */

/**
 * mem_init - initialize the memory system model.
 */
void mem_init(void);
void print_mem(void);

/**
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void);

/**
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk(void);

/**
 * mem_sbrk - simple model of the sbrk function. Extends the heap
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 * @return starting address of new area, or -1 if out of memory
 */
void *mem_sbrk(int incr);

/**
 * mem_heap_lo - return address of the first heap byte.
 *
 * @return address of the first heap byte
 */
void *mem_heap_lo(void);

/**
 * mem_heap_hi - return address of last heap byte.
 *
 * @return address of last heap byte
 */
void *mem_heap_hi(void);

/**
 * mem_heapsize() - returns the heap size in bytes.
 *
 * @return heap size in bytes
 */
size_t mem_heapsize(void);

/**
 * mem_pagesize() - returns the page size of the system.
 *
 * @return page size of the system
 */
size_t mem_pagesize(void);


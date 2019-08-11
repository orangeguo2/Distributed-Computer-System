/*
 * memlib.c - a module that simulates the memory system.  Needed because it 
 *            allows us to interleave calls from the student's malloc package 
 *            with the system's malloc package in libc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
//#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
/*
 * Default maximum heap size in bytes
 */
#ifndef MAX_HEAP
#define MAX_HEAP (20*(1<<20))  /* 20 MB */
#endif

/* private variables */
/** points to first byte of heap */
static void *mem_start_brk = NULL;

/** points to last byte of heap */
static void *mem_brk = NULL;

/** largest legal heap address */
static void *mem_max_addr = NULL;

void print_mem(void){
	   fprintf(stderr, "mem_start_brk is %p \n",mem_start_brk);
    fprintf(stderr, "mem_brk is %p\n",mem_brk);
    fprintf(stderr, "mem_max address is %p\n",mem_max_addr);

}

/**
 * mem_init - initialize the memory system model.
 */
void mem_init(void) {
	if (mem_start_brk == NULL) {
		/* allocate the storage we will use to model the available VM */
		mem_start_brk = (char *)malloc(MAX_HEAP);
		if (mem_start_brk == NULL) {
//	  		fprintf(stderr, "mem_init_vm: malloc error\n");
			exit(1);
		}

		mem_max_addr = mem_start_brk + MAX_HEAP;  /* max legal heap address */
		mem_brk = mem_start_brk;                  /* heap is empty initially */
//		fprintf(stderr, "\nafter mem-init, the very begining \n");
//		print_mem();
//	    fprintf(stderr, "mem_brk is %p",mem_brk);
//	    fprintf(stderr, "mem_max address is %p",mem_max_addr);
//	    fprintf(stderr, "mem_start_brk is %p",mem_start_brk);
	}
}

/**
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void) {
    free(mem_start_brk);
    mem_start_brk = mem_max_addr = mem_brk = 0;
}

/**
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk() {
    mem_brk = mem_start_brk;
}

/**
 * mem_sbrk - simple model of the sbrk function. Extends the heap 
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 *
 * @param incr amount of memory to extend heap in bytes
 */
void *mem_sbrk(int incr) {
    char *old_brk = mem_brk;

    // initialize memory if not already initialized
    if (mem_start_brk == NULL) {
    	mem_init();
    }
//    fprintf(stderr, "\n print mem inside mem_sbrk after more core\n ");
//    print_mem();
//    fprintf(stderr, "\n inside mem_sbrk after more core the incr is %d\n ", incr);

//    fprintf(stderr, "\nincr is %d\n",incr);
//    fprintf(stderr, "mem_brk is %p",mem_brk);
//    fprintf(stderr, "mem_max address is %p",mem_max_addr);
//    fprintf(stderr, "mem_start_brk is %p",mem_start_brk);
//    if(incr < 0)
//    	fprintf(stderr, "incr < 0\n");
//    else{
//    if((mem_brk + incr) > mem_max_addr)
//    	fprintf(stderr, "(mem_brk + incr) > mem_max_addr\n");
//
//    }
    if ( (incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
		errno = ENOMEM;
//		fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
		return (void *)-1;
    }
    mem_brk += incr;
//    fprintf(stderr, "\n print mem inside mem_sbrk after increase mem_brk\n ");
//    print_mem();
    return (void *)old_brk;
}

/**
 * mem_heap_lo - return address of the first heap byte.
 *
 * @return address of the first heap byte
 */
void *mem_heap_lo() {
    return (void *)mem_start_brk;
}

/**
 * mem_heap_hi - return address of last heap byte.
 *
 * @return address of last heap byte
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/**
 * mem_heapsize() - returns the heap size in bytes.
 *
 * @returns the heap size in bytes
 */
size_t mem_heapsize() 
{
    return (size_t)(mem_brk - mem_start_brk);
}

/**
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}

///*
// * mm-malloc_heap.c - Uses C malloc/free/realloc functions directly
// *
// * This implementation is for comparison purposes with the
// * actual C malloc/free/realloc functions.
// */
//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <unistd.h>
//#include <stddef.h>
//#include <string.h>
//
//#include "mm_heap.h"
//#include "memlib.h"
//
//
///**
// * Initialize memory allocator
// */
//void mm_init() {
//}
//
///**
// * Reset memory allocator
// */
//void mm_reset() {
//}
//
///**
// * De-initialize memory allocator
// */
//void mm_deinit() {
//}
//
//
///*
// * mm_malloc - Allocate a block by incrementing the brk pointer.
// *     Always allocate a block whose size is a multiple of the alignment.
// */
//void *mm_malloc(size_t size) {
//	return malloc(size);
//}
//
///*
// * mm_free - Freeing a block does nothing.
// */
//void mm_free(void *ptr) {
//	free(ptr);
//}
//
///*
// * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
// */
//void *mm_realloc(void *ptr, size_t size) {
//	return realloc(ptr, size);
//}
//
///**
// * Calculate the total amount of available free memory.
// *
// * @return the amount of free memory in bytes
// */
//size_t mm_getfree(void) {
//	return 0;  // never frees
//}
//
//
//
//
//
//
//
//
//
//
//
//
//

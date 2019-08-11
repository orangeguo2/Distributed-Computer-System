///*
// * mm-simple_heap.c - The fastest, least memory-efficient malloc package.
// *
// * In this naive approach, a block is allocated by simply incrementing
// * the brk pointer.  A block is pure payload. There are no headers or
// * footers.  Blocks are never coalesced or reused. Realloc is
// * implemented directly using mm_malloc and mm_free.
// */
//#include <stdio.h>
//#include <stdlib.h>
//#include <assert.h>
//#include <unistd.h>
//#include <stddef.h>
//#include <string.h>
//#include "errno.h"
//#include "mm_heap.h"
//#include "memlib.h"
//
///* single word (4) or double word (8) alignment */
//#define ALIGNMENT sizeof(max_align_t)
//
///* rounds up to the nearest multiple of ALIGNMENT */
//#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
//
//
//#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
//
///**
// * Initialize memory allocator
// */
//void mm_init() {
//	mem_init();
//}
//
///**
// * Reset memory allocator
// */
//void mm_reset() {
//	mem_reset_brk();
//}
//
///**
// * De-initialize memory allocator
// */
//void mm_deinit() {
//	mem_deinit();
//}
//
//
///*
// * mm_malloc - Allocate a block by incrementing the brk pointer.
// *     Always allocate a block whose size is a multiple of the alignment.
// */
//void *mm_malloc(size_t size) {
//    int newsize = ALIGN(size + SIZE_T_SIZE);
//    void *p = mem_sbrk(newsize);
//    if ((int)p == -1) {
//    	errno = ENOMEM;
//    	return NULL;
//    } else {
//        *(size_t *)p = size;
//        return p + SIZE_T_SIZE;
//    }
//}
//
///*
// * mm_free - Freeing a block does nothing.
// */
//void mm_free(void *ptr) {
//}
//
///*
// * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
// */
//void *mm_realloc(void *ptr, size_t size) {
//    void *oldptr = ptr;
//    void *newptr;
//    size_t copySize;
//
//    newptr = mm_malloc(size);
//    if (newptr == NULL) {
//    	return mm_malloc(size);
//    }
//    copySize = *(size_t *)(oldptr - SIZE_T_SIZE);
//    if (size < copySize) {
//      copySize = size;
//    }
//    memcpy(newptr, oldptr, copySize);
//    mm_free(oldptr);
//    return newptr;
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

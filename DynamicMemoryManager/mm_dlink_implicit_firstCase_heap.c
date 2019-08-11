///*
// * mm_kr_heap.c
// *
// * Based on C dynamic memory manager code from
// * Brian Kernighan and Dennis Richie (K&R)
// *
// *  @since Feb 13, 2019
// *  @author philip gust
// */
//
//
//#include <stdio.h>
//#include <unistd.h>
//#include <stddef.h>
//#include <string.h>
//#include "memlib.h"
//#include "mm_heap.h"
//
//
///** Header and tail information for all blocks */
//typedef union header {      /* block header */
//	size_t size;            /* size of this block including two header */
//	/* the basic unit is one Header
//	 * total block size is always a multiple of two headers (at head
//	 * and tail) so the size is always even.
//	 * lower bit of the size indicates that
//	 * the block has been allocated if the
//	 * bit is "1" and is free if the bit is "0".   */
//	max_align_t x;          /* force alignment to max align boundary */
//} Header;
//
//// forward declarations
//static Header *morecore(size_t);
//void visualize(const char*);
//
///** total memory in chunks */
//static size_t totmem = 0;
//
///** point to first byte after last tail of all chunks */
//static Header* bottom=NULL;
//
///** point to first byte of header of all chunks */
//static Header * begin=NULL;
//
//
///**
// * Initialize memory allocator
// */
//void mm_init() {
//	mem_init();
//	char* bridge=mem_heap_lo();
//	begin=(Header*)bridge;
//	begin->size=0;
//	bottom=begin;
//}
//
///**
// * Reset memory allocator
// */
//void mm_reset() {
//	mem_reset_brk();
//	char* bridge=mem_heap_lo();
//	begin=(Header*)bridge;
//	begin->size=0;
//	bottom=begin;
//	totmem = 0;
//}
//
///**
// * De-initialize memory allocator
// */
//void mm_deinit() {
//	mem_deinit();
//	char* bridge=mem_heap_lo();
//	begin=(Header*)bridge;
//	begin->size=0;
//	bottom=begin;
//}
//
///**
// * Allocation units for nbytes.
// *
// * @param nbytes number of bytes
// * @return number of units for nbytes
// */
//inline static size_t mm_units(size_t nbytes) {
//    /* smallest count of Header-sized memory chunks */
//    /*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//	size_t unit2 = (nbytes + 2*sizeof(Header) - 1) / (2*sizeof(Header)) + 1;
//	return 2*unit2;
//}
//
///**
// * Allocation bytes for nunits.
// *
// * @param nunits number of units
// * @return number of bytes for nunits
// */
//inline static size_t mm_bytes(size_t nunits) {
//    return nunits * sizeof(Header);
//}
//
///**
// * Allocates size bytes of memory and returns a pointer to the
// * allocated memory, or NULL if request storage cannot be allocated.
// *
// * @param nbytes the number of bytes to allocate
// * @return pointer to allocated memory or NULL if not available.
// */
//void *mm_malloc(size_t nbytes) {
//
//    if (mem_heap_lo() == NULL) {
//    	mm_init();
//    }
//
//    /* smallest count of 2 Header-sized memory chunks */
//    /*  (+1 additional chunk for the Header and tail itself) needed to hold nbytes */
//    size_t nunits = mm_units(nbytes);
////	fprintf(stderr, "  \n the actural size to allocate inside header and tail is %zu\n",mm_bytes( nunits));
//
//
//    /* traverse all block to find a free block */
//    for (Header *p = begin; p<=bottom; ) {
////    	fprintf(stderr, "  \n go inside for loop\n");
////    	fprintf(stderr, "  \n the p size for this block is %zu\n",mm_bytes( p->size));
////    	fprintf(stderr, "  \n the address of p is %p\n",p);
//
//        if (p->size >= nunits && !(p->size&1)) {          /* big enough and not allocate */
////        	fprintf(stderr, "go inside correct \ngo inside correct \ngo inside correct \ngo inside correct \ngo inside correct \n");
//        	if (p->size == nunits) {       /* exactly */
////        		fprintf(stderr, "  \n go inside the equal p->size\n");
//                p->size=p->size|1;
//            } else {                         /* split allocate tail end */
////            	fprintf(stderr, "  \n go inside the bigger p->size\n");
//                /* adjust the size to split the block */
//                p->size -= nunits;
//                Header* tailSize=p+p->size-1;
//                 tailSize->size=p->size;
////             	fprintf(stderr, "  \n the p size for still free block is %zu\n",mm_bytes( p->size));
//
//                /* find the address to return */
//                p += p->size;
//                p->size = nunits;
//                tailSize=p+p->size-1;
//                tailSize->size=p->size;
////             	fprintf(stderr, "  \n the p size before adjust allocate flag is %zu\n",mm_bytes( p->size));
//
//                p->size=p->size|1;
////             	fprintf(stderr, "  \n the p size after adjust allocate flag is %zu\n",mm_bytes( p->size));
//
//            }
////        	  fprintf(stderr, "  \n  mm_getfree(void) is %zu\n", mm_getfree() );
////        	        fprintf(stderr, "\n print mem after malloc\n ");
////        	        print_mem();
////        	            fprintf(stderr, "\n begin is %p\n ",begin);
////        	            fprintf(stderr, "\n bottom is %p\n ",bottom);
////        	            fprintf(stderr, "\n allocated block size is %zu\n ",mm_bytes(p->size));
//            return (void *)(p+1);
//        }
//
//        /* back where we started and nothing found - we need to allocate */
//        if (p==bottom) {                    /* wrapped around free list */
////        	fprintf(stderr, "  \n go inside bottom if p->size\n");
//        	p = morecore(nunits);
//        	if (p == NULL) {
////        		fprintf(stderr, "go inside p==bottom error");
//                return (void*)NULL;                /* none left */
//            }
//        }
//        else{
//        	if(!(p->size&1))
//        	p+=p->size ;
//        	else
//        		p+=p->size^1 ;
//        }
//
//    }
////    fprintf(stderr, "  \n  after whole for loop mm_getfree(void) is %zu\n", mm_getfree() );
////        fprintf(stderr, "\n print mem after malloc\n ");
////        print_mem();
////            fprintf(stderr, "\n begin is %p\n ",begin);
////            fprintf(stderr, "\n bottom is %p\n ",bottom);
//
//}
//
//
///**
// * Deallocates the memory allocation pointed to by ptr.
// * if ptr is a NULL pointer, no operation is performed.
// *
// * @param ap the allocated block to free
// */
//void mm_free(void *ap) {
//
//
//    if (ap == NULL) {
//        return;
//    }
//
//
//    Header *bp = (Header *)ap - 1;                 /* point to block header */
//
//    if (bp->size == 0 || bp->size > totmem) {  /* cannot happen */
//        return;
//    }
//   if(!(bp->size&1))
//		return;
//
//    bp->size=bp->size^1;
//
//}
//
///**
// * Reallocates size bytes of memory and returns a pointer to the
// * allocated memory, or NULL if request storage cannot be allocated.
// *
// * @param ap the currently allocated storage
// * @param nbytes the number of bytes to allocate
// * @return pointer to allocated memory or NULL if not available.
// */
//void *mm_realloc(void *ap, size_t size) {
//    Header *bp = (Header *)ap - 1;                 /* point to block header */
//
//    if (bp->size == 0 || bp->size > totmem) {
//        return NULL;
//    }
//
//    /* smallest count of Header-sized memory chunks */
//    /*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//    size_t nunits = mm_units(size);
//    if (bp->size >= nunits) {
//    	return ap;
//    }
//    size_t oldsize = bp->size;  // save before malloc changes it
//
//    void *newap = mm_malloc(size);
//    if (newap == NULL) {
//    	return NULL;
//    }
//    // copy to new storage and free old storage
//    size_t nbytes = mm_bytes(oldsize - 1);
//    memcpy(newap, ap, nbytes);
//    mm_free(ap);
//
//    return newap;
//}
//
///**
// * Request additional memory to be added to this process.
// *
// * @param nu the number of Header-chunks to be added
// */
//static Header *morecore(size_t nu) {
//	// nalloc based on page size
//	size_t nalloc = mem_pagesize()/(2*sizeof(Header));
//	nalloc=nalloc*2;
//    /* get at least NALLOC Header-chunks from the OS */
//    if (nu < nalloc) {
//        nu = nalloc;
//    }
//
//    size_t nbytes = mm_bytes(nu); // number of bytes
////    fprintf(stderr, "\n print mem before mem_sbrk on more core\n ");
////    print_mem();
//    char *cp = (char *) mem_sbrk(nbytes);
//    if (cp == (char *) -1) {                 /* no space at all */
////    	fprintf(stderr, "go inside more core error");
//        return NULL;
//    }
//    totmem += nu;                           /* keep track of allocated memory */
//    Header *up = (Header *) cp;
//    up->size = nu;
//    Header* tail = up+up->size-1;
//    tail->size=up->size;
//    bottom = tail+1;
//
//    /* add the free space to the circular list */
//    void *n = (void *)(up+1);
//    mm_free(n);
////    fprintf(stderr, "\n cp is %p\n ",cp);
////    fprintf(stderr, "\n up is %p\n ",up);
////    fprintf(stderr, "\n tail is %p\n ",tail);
////    fprintf(stderr, "\n bottom is %p\n ",bottom);
////    fprintf(stderr, "\n n is %p\n ",n);
//
//    return up;
//}
//
///**
// * Print the free list (educational purpose) *
// *
// * @msg the initial message to print
// */
//void visualize(const char* msg) {
//    Header *tmp;
//
//    fprintf(stderr, "\n--- Free list after \"%s\":\n", msg);
//
//    if (mem_heap_lo() == NULL) {                   /* does not exist */
//        fprintf(stderr, "    List does not exist\n\n");
//        return;
//    }
//
//    if (mem_heap_lo() == mem_heap_hi()) {          /* self-pointing list = empty */
//        fprintf(stderr, "    List is empty\n\n");
//        return;
//    }
//
//    tmp = mem_heap_lo() ;                           /* find the start of the list */
//    char *str = "    ";
//    do {           /* traverse the list */
//        fprintf(stderr, "%sptr: %10p size: %-3lu\n", str, (void *)tmp, tmp->size);
//        str = " -> ";
//        tmp += tmp->size;
//    }  while (tmp > bottom);
//
//    fprintf(stderr, "--- end\n\n");
//}
//
//
///**
// * Calculate the total amount of available free memory.
// *
// * @return the amount of free memory in bytes
// */
//size_t mm_getfree(void) {
//    if (mem_heap_lo() == NULL) {
//        return 0;
//    }
//
//    Header *tmp = mem_heap_lo();
//    size_t res = tmp->size;
//
//    while (tmp > bottom) {
//        tmp += tmp->size;
//        res += tmp->size;
//    }
//
//    return mm_bytes(res);
//}

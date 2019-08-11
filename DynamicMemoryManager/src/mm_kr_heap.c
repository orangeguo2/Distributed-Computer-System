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
//#include "errno.h"
//
///** Header information for allocated blocks */
//typedef union header {          /* block header */
//    struct {
//        union header *ptr;      /* next block if on free list */
//        size_t size;          	/* size of this block including header */
//                                /* measured in multiple of header size */
//    } s;
//    max_align_t x;              /* force alignment to max align boundary */
//} Header;
//
//// forward declarations
//static Header *morecore(size_t);
//void visualize(const char*);
//
///** total memory in chunks */
//static size_t totmem = 0;
//
///** Empty list to get started */
//static Header base;
//
///** Start of free memory list */
//static Header *freep = NULL;
//
///**
// * Initialize memory allocator
// */
//void mm_init() {
//	mem_init();
//
//    base.s.ptr = freep = &base;
//    base.s.size = 0;
//
//}
//
///**
// * Reset memory allocator
// */
//void mm_reset() {
//	mem_reset_brk();
//
//	base.s.ptr = freep = &base;
//    base.s.size = 0;
//}
//
///**
// * De-initialize memory allocator
// */
//void mm_deinit() {
//	mem_deinit();
//
//	base.s.ptr = freep = &base;
//    base.s.size = 0;
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
//    return (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
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
//    if (freep == NULL) {
//    	mm_init();
//    }
//
//    Header *prevp = freep;
//
//    /* smallest count of Header-sized memory chunks */
//    /*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//    size_t nunits = mm_units(nbytes);
//
//
//    /* traverse the circular list to find a block */
//    for (Header *p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
//        if (p->s.size >= nunits) {          /* big enough */
//            if (p->s.size == nunits) {       /* exactly */
//                prevp->s.ptr = p->s.ptr;
//            } else {                         /* split allocate tail end */
//                /* adjust the size to split the block */
//                p->s.size -= nunits;
//                /* find the address to return */
//                p += p->s.size;
//                p->s.size = nunits;
//            }
//            freep = prevp;  /* move the head */
//            return (void *)(p+1);
//        }
//
//        /* back where we started and nothing found - we need to allocate */
//        if (p == freep) {                    /* wrapped around free list */
//        	p = morecore(nunits);
//        	if (p == NULL) {
//			errno = ENOMEM;
//                return NULL;                /* none left */
//            }
//        }
//    }
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
//    if (ap == NULL) {
//        return;
//    }
//
//    Header *bp = (Header *)ap - 1;                 /* point to block header */
//
//    if (bp->s.size == 0 || bp->s.size > totmem) {  /* cannot happen */
//        return;
//    }
//
//    /* look where to insert the free space */
//
//    /* (bp > p && bp < p->s.ptr)    => between two nodes */
//    /* (p > p->s.ptr)               => this is the end of the list */
//    /* (p == p->p.ptr)              => list is one element only */
//    Header *p = freep;
//    for ( ; !(bp > p && bp < p->s.ptr); p = p->s.ptr) {
//        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) {
//       //     /* freed block at start or end of arena */
//            break;
//        }
//	}
//
//    if (bp + bp->s.size == p->s.ptr) {      /* join to upper nbr */
//    /* the new block fits perfect up to the upper neighbor */
//
//        /* merging up: adjust the size */
//        bp->s.size += p->s.ptr->s.size;
//        /* merging up: point to the second next */
//        bp->s.ptr = p->s.ptr->s.ptr;
//
//    } else {
//        /* set the upper pointer */
//        bp->s.ptr = p->s.ptr;
//    }
//
//    if (p + p->s.size == bp) {              /* join to lower nbr */
//    /* the new block fits perfect on top of the lower neighbor */
//
//        /* merging below: adjust the size */
//        p->s.size += bp->s.size;
//        /* merging below: point to the next */
//        p->s.ptr = bp->s.ptr;
//
//    } else {
//        /* set the lower pointer */
//        p->s.ptr = bp;
//    }
//
//    /* reset the start of the free list */
//    freep = p;
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
//if (ap == NULL) {
//            return mm_malloc(size);
//        }
//    Header *bp = (Header *)ap - 1;                 /* point to block header */
//
//    if (bp->s.size == 0 || bp->s.size > totmem) {
//        return NULL;
//    }
//
//    /* smallest count of Header-sized memory chunks */
//    /*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//    size_t nunits = mm_units(size);
//    if (bp->s.size >= nunits) {
//    	return ap;
//    }
//    size_t oldsize = bp->s.size;  // save before malloc changes it
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
//	size_t nalloc = mem_pagesize()/sizeof(Header);
//    /* get at least NALLOC Header-chunks from the OS */
//    if (nu < nalloc) {
//        nu = nalloc;
//    }
//
//    size_t nbytes = mm_bytes(nu); // number of bytes
//    char *cp = (char *) mem_sbrk(nbytes);
//    if (cp == (char *) -1) {                 /* no space at all */
//        return NULL;
//    }
//    totmem += nu;                           /* keep track of allocated memory */
//
//    Header *up = (Header *) cp;
//    up->s.size = nu;
//
//    /* add the free space to the circular list */
//    void *n = (void *)(up+1);
//    mm_free(n);
//
//    return freep;
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
//    if (freep == NULL) {                   /* does not exist */
//        fprintf(stderr, "    List does not exist\n\n");
//        return;
//    }
//
//    if (freep == freep->s.ptr) {          /* self-pointing list = empty */
//        fprintf(stderr, "    List is empty\n\n");
//        return;
//    }
//
//    tmp = freep;                           /* find the start of the list */
//    char *str = "    ";
//    do {           /* traverse the list */
//        fprintf(stderr, "%sptr: %10p size: %-3lu\n", str, (void *)tmp, tmp->s.size);
//        str = " -> ";
//        tmp = tmp->s.ptr;
//    }  while (tmp->s.ptr > freep);
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
//    if (freep == NULL) {
//        return 0;
//    }
//
//    Header *tmp = freep;
//    size_t res = tmp->s.size;
//
//    while (tmp->s.ptr > tmp) {
//        tmp = tmp->s.ptr;
//        res += tmp->s.size;
//    }
//
//    return mm_bytes(res);
//}

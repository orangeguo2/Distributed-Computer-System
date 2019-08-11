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
//#include <stdio.h>
//#include <unistd.h>
//#include <stddef.h>
//#include <string.h>
//#include "memlib.h"
//#include "mm_heap.h"
//#include "errno.h"
//
///** Header and tail information for all blocks */
//typedef union header { /* block header */
//	size_t size; /* size of this block including two header */
//	/* the basic unit is one Header
//	 * total block size is always a multiple of two headers (at head
//	 * and tail) so the size is always even.
//	 * lower bit of the size indicates that
//	 * the block has been allocated if the
//	 * bit is "1" and is free if the bit is "0".   */
//	max_align_t x; /* force alignment to max align boundary */
//} Header;
//
////use the space in the block payload area to store pointers to the next
//typedef struct free_pointer {
//	struct free_pointer* next;
//	struct free_pointer* prev;
//} FreePointer;
//
//typedef struct dummy_node {
//	Header header;
//	FreePointer frp;
//} Dummy;
//
//// forward declarations
//static Header *morecore(size_t);
//void visualize(const char*);
//
///** total memory in chunks */
//static size_t totmem = 0;
//
///** Empty list to get started */
//static Dummy base;
//
///** Start of free memory list */
//static FreePointer *freep = NULL;
//
///**
// * Initialize memory allocator
// */
//void mm_init() {
////	fprintf(stderr, "\n mm-init() called and calling mem_init \n");
//
//	mem_init();
//	base.header.size = 0;
//	base.frp.next = &(base.frp);
//	base.frp.prev = &(base.frp);
//	freep = &(base.frp);
//
////	print_mem();
////	fprintf(stderr, "\n base is %p \n", &base);
////	fprintf(stderr, "\n base header is %p \n", &(base.header));
////	fprintf(stderr, "\n base frp is %p \n", &(base.frp));
////	fprintf(stderr, "\n base frp next is base? 0 for false: %d \n",
////			base.frp.next == &(base.frp));
////	fprintf(stderr, "\n base frp prev is base?0 for false : %d \n",
////			base.frp.prev == &(base.frp));
//
////	fprintf(stderr, "\n base.prev is %p \n", base.prev);
//
//}
//
///**
// * Reset memory allocator
// */
//void mm_reset() {
//	mem_reset_brk();
//	base.header.size = 0;
//	base.frp.next = &(base.frp);
//	base.frp.prev = &(base.frp);
//	freep = &(base.frp);
//}
//
///**
// * De-initialize memory allocator
// */
//void mm_deinit() {
//	mem_deinit();
//	base.header.size = 0;
//	base.frp.next = &(base.frp);
//	base.frp.prev = &(base.frp);
//	freep = &(base.frp);
//}
//
///**
// * Allocation units for nbytes. in the base of one header
// *
// * @param nbytes number of bytes
// * @return number of units for nbytes
// */
//inline static size_t mm_units(size_t nbytes) {
//	/* smallest count of Header-sized memory chunks */
//	/*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//	size_t unit2 = (nbytes + sizeof(FreePointer) + 2 * sizeof(Header) - 1)
//			/ (2 * sizeof(Header)) + 1;
//	return 2 * unit2;
//}
//
///**
// * Allocation bytes for nunits.
// *
// * @param nunits number of units
// * @return number of bytes for nunits
// */
//inline static size_t mm_bytes(size_t nunits) {
//	return nunits * sizeof(Header);
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
//	if (freep == NULL) {
//		mm_init();
//	}
//	/* smallest count of Header-sized memory chunks */
//	/*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//	size_t nunits = mm_units(nbytes);
////	fprintf(stderr, "  \n size of header is %zu\n", sizeof(Header));
////	fprintf(stderr, "  \n size of FreePointer is %zu\n", sizeof(FreePointer));
////	fprintf(stderr,
////			"  \n the actural size to allocate include fpp, header and tail is %zu\n",
////			mm_bytes(nunits));
////	print_mem();
////	fprintf(stderr, "  \n freep is %p\n", freep);
//	/* traverse the circular list to find a block */
//	for (FreePointer *p = freep->next;; p = p->next) {
////		fprintf(stderr, "  \n the address of p is %p\n", p);
//		if (p != &(base.frp)) {
//			Header* curHeader = (Header*) p - 1;
//			Header* nextHeader = curHeader;
//			if ((curHeader->size == nunits)
//					|| ((curHeader->size > nunits)
//							&& ((mm_bytes(curHeader->size - nunits)
//									> (sizeof(Header) * 2 + sizeof(FreePointer)))))) { /* big enough */
////				fprintf(stderr, "  \n  when find bigger enough p next is  %p\n",
////						p->next);
////
////				fprintf(stderr, "  \n  p prev is  %p\n", p->prev);
//				if (curHeader->size == nunits) { /* exactly */
//					p->prev->next = p->next;
//					p->next->prev = p->prev;
//
//					Header* tail = curHeader + curHeader->size - 1;
//					curHeader->size = curHeader->size | 1;
//					tail->size = curHeader->size;
//
//				} else { /* split allocate tail end */
//					/* adjust the size to split the block */
////					fprintf(stderr, "  \n  block size is bigger than \n");
//					curHeader->size -= nunits;
//					Header* tail = curHeader + curHeader->size - 1;
//					tail->size = curHeader->size;
//
//					/* find the address to return */
//					nextHeader = curHeader + curHeader->size;
//					nextHeader->size = nunits;
//					Header* nextTail = nextHeader + nextHeader->size - 1;
//					nextHeader->size = nunits | 1;
//					nextTail->size = nextHeader->size;
//				}
//
//				freep = p->prev; /* move the head */
//
////				fprintf(stderr, "  \n after all alloc p next is  %p\n",
////						p->next);
////
////				fprintf(stderr, "  \n  p prev is  %p\n", p->prev);
////				fprintf(stderr,
////						"  \n  after allocate \n mm_getfree(void) is %zu\n",
////						mm_getfree());
////				fprintf(stderr,
////						"  \n  allocated address header start at nextHeader %p \n",
////						nextHeader);
////				fprintf(stderr, "  \n  allocated size is %zu \n",
////						mm_bytes(nextHeader->size ^ 1));
////
////				visualize("the malloc");
////				fprintf(stderr, "  \n  return address of malloc is %p\n",
////						nextHeader + 1);
////				fprintf(stderr, "  \n  return size of block is %lu\n",
////						nextHeader->size);
////				fprintf(stderr,
////						"  \n  return size of block before flag is %lu\n",
////						nextHeader->size ^ 1);
////				fprintf(stderr, "  \n  freep is set to %p\n", freep);
//
//				return (void *) (nextHeader + 1);
//			}
//		}
//		/* back where we started and nothing found - we need to allocate */
//		if (p == freep) { /* wrapped around free list */
////			fprintf(stderr,
////					"\n no free bloack is suitable, call more core \n ");
//
//			morecore(nunits);
//			p = freep;
//			if (p == NULL) {
//				errno = ENOMEM;
//				return NULL; /* none left */
//			}
//		}
//
//	}
//}
//
///**
// * Deallocates the memory allocation pointed to by ptr.
// * if ptr is a NULL pointer, no operation is performed.
// * when passed a pointer to memory that is not in the
// * heap memory pool, or to memory that has already been freed
// * return from mm_free() with the global variable errno set
// * to a constant EFAULT
// *
// * @param ap the allocated block to free
// */
//void mm_free(void *ap) {
////	fprintf(stderr, "\n free:pass in ap to free is  %p\n ", ap);
//
////	spcial case handle
//	if (ap == NULL) {
//		return;
//	}
//	Header* hp = (Header *) ap - 1;
//	if (hp->size == 0 || hp->size > totmem) { /* cannot happen */
//		return;
//	}
//
//
////	fprintf(stderr, "\n hp size just assign is  %zu\n ", mm_bytes(hp->size));
//
//	if (hp->size & 1) {
////		fprintf(stderr, "\n free the last bit \n ");
//
//		hp->size = hp->size ^ 1;
////		fprintf(stderr, "\n hp size after free is  %zu\n ", mm_bytes(hp->size));
//	}
//
////	add some safeguards to ensure that it is not possible to free storage that was already freed or storage that is not part of the heap.
//	FreePointer *testp = freep;
//	do{
//		if(testp==ap){
//			errno = EFAULT;
//			return;
//		}
//		testp=testp->next;
//	}while(testp!=freep);
//
//	if(ap<mem_heap_lo()||ap>mem_heap_hi()){
//		errno = EFAULT;
//		return;
//	}
//
//	FreePointer *p = freep;
//	FreePointer *fp = (FreePointer *) ap;
////	fprintf(stderr, "\n hp is  %p\n ", hp);
////	fprintf(stderr, "\n fp is  %p\n ", fp);
//	for (; !(fp > p && fp < p->next); p = p->next) {
//		if (p >= p->next && (fp > p || fp < p->next)) {
//			//     /* freed block at start or end of arena */
////			fprintf(stderr, "\n  break because of remote case \n ");
//			break;
//		}
//	}
//
////	fprintf(stderr, "\n when break p is  %p\n ", p);
////	fprintf(stderr, "\n when break p.next is  %p\n ", p->next);
////	fprintf(stderr, "\n when break p.nextnext is  %p\n ", p->next->next);
////	fprintf(stderr, "\n when break p.nextpre is  %p\n ", p->next->prev);
////	fprintf(stderr, "\n when break p.prev is  %p\n ", p->prev);
//
//	Header* headerBp = (Header *) p - 1;
//	Header* headerBpplus = (Header *) (p->next) - 1;
//
////	fprintf(stderr, "\n headerBp is  %p\n ", headerBp);
////	fprintf(stderr, "\n headerBp size is %lu \n", mm_bytes(headerBp->size));
////
////	fprintf(stderr, "\n  headerBpplus is  %p\n ", headerBpplus);
//	if (hp + hp->size == headerBpplus) { /* join to upper nbr */
//		/* the new block fits perfect up to the upper neighbor */
//
//		/* merging up: adjust the size */
//		hp->size += headerBpplus->size;
//		//tail
//		(headerBpplus + headerBpplus->size - 1)->size = hp->size;
//
////		fprintf(stderr, "\n fp->header size is %zu \n",
////					mm_bytes(headerBp->size));
////			fprintf(stderr, "\n fp->tail size is %zu \n",
////					mm_bytes((headerBp + headerBp->size - 1)->size));
////
////
////		/* merging up: point to the second next */
////		fprintf(stderr, "\n go upper \n");
////		fprintf(stderr, "\n p is  %p\n ", p);
////		fprintf(stderr, "\n fp is  %p\n ", fp);
////		fprintf(stderr, "\n p.next is  %p\n ", p->next);
////		fprintf(stderr, "\n p.prev is  %p\n ", p->prev);
////		fprintf(stderr, "\n p.next next  is  %p\n ", p->next->next);
////		fprintf(stderr, "\n p.next prev  is  %p\n ", p->next->prev);
////		fprintf(stderr, "\n fp.next is  %p\n ", fp->next);
////			fprintf(stderr, "\n fp.prev is  %p\n ", fp->prev);
//		fp->next = p->next->next;
//		p->next->next->prev = fp;
//
//	} else {
////		fprintf(stderr, "\n go upper else \n");
////		fprintf(stderr, "\n p is  %p\n ", p);
////		fprintf(stderr, "\n fp is  %p\n ", fp);
////		fprintf(stderr, "\n p.next is  %p\n ", p->next);
////		fprintf(stderr, "\n p.prev is  %p\n ", p->prev);
////		fprintf(stderr, "\n p.next next  is  %p\n ", p->next->next);
////		fprintf(stderr, "\n p.next prev  is  %p\n ", p->next->prev);
////		fprintf(stderr, "\n fp.next is  %p\n ", fp->next);
////			fprintf(stderr, "\n fp.prev is  %p\n ", fp->prev);
//		fp->next = p->next;
//		p->next->prev = fp;
//
//	}
//
//	if (headerBp + headerBp->size == hp) { /* join to lower nbr */
//		/* the new block fits perfect on top of the lower neighbor */
//
//		/* merging below: adjust the size */
//		headerBp->size += hp->size;
//		(headerBp + headerBp->size - 1)->size = headerBp->size;
//
//		/* merging up: point to the second next */
////		fprintf(stderr, "\n go upper \n");
////		fprintf(stderr, "\n p is  %p\n ", p);
////		fprintf(stderr, "\n fp is  %p\n ", fp);
////		fprintf(stderr, "\n p.next is  %p\n ", p->next);
////		fprintf(stderr, "\n p.prev is  %p\n ", p->prev);
////		fprintf(stderr, "\n p.next next  is  %p\n ", p->next->next);
////		fprintf(stderr, "\n p.next prev  is  %p\n ", p->next->prev);
////		fprintf(stderr, "\n fp.next is  %p\n ", fp->next);
////			fprintf(stderr, "\n fp.prev is  %p\n ", fp->prev);
//		p->next = fp->next;
//		fp->next->prev = p;
//
////		fprintf(stderr, "\n p->header size is %zu \n",
////				mm_bytes(headerBp->size));
////		fprintf(stderr, "\n p->tail size is %zu \n",
////				mm_bytes((headerBp + headerBp->size - 1)->size));
//
//	} else {
////		fprintf(stderr, "\n go lower else corner \n");
////		fprintf(stderr, "\n p is  %p\n ", p);
////		fprintf(stderr, "\n fp is  %p\n ", fp);
////		fprintf(stderr, "\n p.next is  %p\n ", p->next);
////		fprintf(stderr, "\n p.prev is  %p\n ", p->prev);
////		fprintf(stderr, "\n p.next next  is  %p\n ", p->next->next);
////		fprintf(stderr, "\n p.next prev  is  %p\n ", p->next->prev);
////		fprintf(stderr, "\n fp.next is  %p\n ", fp->next);
////			fprintf(stderr, "\n fp.prev is  %p\n ", fp->prev);
//
//		p->next = fp;
//		fp->prev = p;
//
//	}
//
//	/* reset the start of the free list */
//	freep = p;
////	fprintf(stderr, "\n after all fee fp next is  %p\n ", fp->next);
////	fprintf(stderr, "\n fp prev is  %p\n ", fp->prev);
////	fprintf(stderr, "\n p is  %p\n ", p);
////	fprintf(stderr, "\n p->next is  %p\n ", p->next);
////	fprintf(stderr, "\n p->next->prev is  %p\n ", p->next->prev);
////
////	fprintf(stderr, "\n  freep is  %p\n ", freep);
//}
//
///**
// * Reallocates size bytes of memory and returns a pointer to the
// * allocated memory, or NULL if request storage cannot be allocated.
// *
// * * when passed a pointer to memory that is not in the
// * heap memory pool, or to memory that has already been freed
// * return from mm_free() with the global variable errno set
// * to a constant EFAULT
// *
// * @param ap the currently allocated storage
// * @param nbytes the number of bytes to allocate
// * @return pointer to allocated memory or NULL if not available.
// */
//void *mm_realloc(void *ap, size_t size) {
//	if (ap == NULL) {
//		return mm_malloc(size);
//	}
//	Header *hp = (Header *) ap - 1; /* point to block header */
//	if (hp->size == 0 || hp->size > totmem) {
//		return NULL;
//	}
//
//	//	add some safeguards to ensure that it is not possible to free storage that was already freed or storage that is not part of the heap.
//		FreePointer *testp = freep;
//		do{
//			if(testp==ap){
//				errno = EFAULT;
//				return NULL;
//			}
//			testp=testp->next;
//		}while(testp!=freep);
//
//		if(ap<mem_heap_lo()||ap>mem_heap_hi()){
//			errno = EFAULT;
//			return NULL;
//		}
//
//	/* smallest count of Header-sized memory chunks */
//	/*  (+1 additional chunk for the Header itself) needed to hold nbytes */
//	size_t nunits = mm_units(size);
//	if (hp->size >= nunits) {
//		return ap;
//	}
//	size_t oldsize = hp->size;  // save before malloc changes it
//
//	void *newap = mm_malloc(size);
//	if (newap == NULL) {
//		return NULL;
//	}
//// copy to new storage and free old storage
//	size_t nbytes = mm_bytes(oldsize - 1) - sizeof(FreePointer);
//	memcpy(newap, ap, nbytes);
//	mm_free(ap);
//
//	return newap;
//}
//
///**
// * Request additional memory to be added to this process.
// *
// * @param nu the number of Header-chunks to be added
// */
//static Header *morecore(size_t nu) {
//// nalloc based on page size
////	fprintf(stderr, "\n mem_pagesize() is %zu\n ", mem_pagesize());
//
//	size_t nalloc = mem_pagesize() / (2 * sizeof(Header));
//	nalloc *= 2;
//	/* get at least NALLOC Header-chunks from the OS */
//	if (nu < nalloc) {
//		nu = nalloc;
//	}
//
//	size_t nbytes = mm_bytes(nu); // number of bytes
////	fprintf(stderr, "\n print mem before mem_sbrk on more core\n ");
////	print_mem();
////	fprintf(stderr, "\n more space to add is %zu\n ", nbytes);
//
//	char *cp = (char *) mem_sbrk(nbytes);
//	if (cp == (char *) -1) { /* no space at all */
//		return NULL;
//	}
//	totmem += nu; /* keep track of allocated memory */
//
//	Header *up = (Header *) cp;
//	up->size = nu;
//
//	Header *tail = up + up->size - 1;
//	tail->size = up->size;
//
//	/* add the free space to the circular list */
//	void *n = (void *) (up + 1);
////	fprintf(stderr, "\n free the memory just alloc, payload starting at %p\n",
////			n);
////	visualize("before add: \n");
//	mm_free(n);
//
////	visualize("\n after add: \n ");
////	fprintf(stderr, "\n head of new alloc up is %p\n ", up);
////	fprintf(stderr, "\n size is %zu\n ", up->size);
////	fprintf(stderr, "\n tail is %p\n ", tail);
////	fprintf(stderr, "\n start of pp -> n is %p\n ", n);
////	fprintf(stderr, "\n freep is %p\n ", freep);
//
////
//	return up;
//}
//
///**
// * Print the free list (educational purpose) *
// *
// * @msg the initial message to print
// */
//void visualize(const char* msg) {
//	FreePointer *tmp;
//
//	fprintf(stderr, "\n--- Free list after \"%s\":\n", msg);
//
//	if (freep == NULL) { /* does not exist */
//		fprintf(stderr, "    List does not exist\n\n");
//		return;
//	}
//
//	if (base.frp.next == &base.frp) { /* self-pointing list = empty */
//		fprintf(stderr, "    List is empty\n\n");
//		return;
//	}
//
//	tmp = base.frp.next; /* find the start of the list */
//	char *str = "    ";
//	while (tmp > &base.frp) { /* traverse the list */
//		fprintf(stderr, "%sptr: %10p size: %-3lu\n", str, (void *) tmp,
//				mm_bytes(((Header*) tmp - 1)->size));
//		str = " -> ";
//		fprintf(stderr, "    \nthe next address is %p\n", tmp->next);
//		tmp = tmp->next;
//	}
//
//	fprintf(stderr, "--- end\n\n");
//}
//
///**
// * Calculate the total amount of available free memory.
// *
// * @return the amount of free memory in bytes
// */
//size_t mm_getfree(void) {
//	if (freep == NULL) {
//		return 0;
//	}
//
//	FreePointer *tmp = &base.frp;
//	size_t res = 0;
//
//	while (tmp->next > tmp) {
//		tmp = tmp->next;
//		if (tmp != &base.frp)
//			res += ((Header*) tmp - 1)->size;
//
//	}
//
//	return mm_bytes(res);
//}

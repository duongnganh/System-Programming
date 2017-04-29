/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this approach, a collection of lists are presented. Each set of
 * lists with length of same range are packed together and the links are
 * managed in free_list. Free blocks are connected via doubly-linked list
 * strucutre and sorted in order of length. Realloc is implemented so that
 * only do memcpy if it is not possible to use the the block right after.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
	/* Team name */
	"20150003-Shine bright like Elaine",
	/* First member's full name */
	"Duong Anh Nguyen",
	/* First member's email address */
	"duong.nguyen@kaist.ac.kr",
	/* Second member's full name (leave blank if none) */
	"",
	/* Second member's email address (leave blank if none) */
	""
};
/* Verbose for mm_checkheap */
#define v 0

/* Turn checkheap on or off */
//#define checkheap(n) mm_checkheap(n)
#define checkheap(n)

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1<<10) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) (GET(bp))
#define PREV_BLKP(bp) (GET(((char *)(bp) + WSIZE)))

/* Given block ptr bp, compute address of physical next and previous blocks */
#define NEXT_PHY_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_PHY_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */
#define LSIZE 31
char *free_list[LSIZE];

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp);
static void mm_checkheap(int verbose);
static void checkblock(void *bp);

/*
 *  * mm_init - Initialize the memory manager
 *   */
int mm_init(void)
{
	/* Create the initial empty heap */
	if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) //line:vm:mm:begininit
		return -1;
	int i = 0;
	while (i < LSIZE){
		free_list[i] = NULL;
		i++;
	}
	PUT(heap_listp, 0);                          /* Alignment padding */
	PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
	PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
	PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
	heap_listp += (2*WSIZE);

	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
	if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
		return -1;	

	checkheap(v);
	return 0;
}

/*
 *  *  * mm_malloc - Allocate a block with at least size bytes of payload
 *   *   */
void *mm_malloc(size_t size)
{
	size_t asize;      /* Adjusted block size */
	size_t extendsize; /* Amount to extend heap if no fit */
	char *bp;

	/* $end mmmalloc */
	if (heap_listp == 0){
		mm_init();
	}
	/* Ignore spurious requests */
	if (size == 0)
		return NULL;

	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= DSIZE)                                         
		asize = 2*DSIZE;                                        
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

	/* Search the free list for a fit */
	if ((bp = find_fit(asize)) != NULL) {  
		place(bp, asize);                 
		return bp;
	}
	/* No fit found. Get more memory and place the block */
	extendsize = MAX(asize,CHUNKSIZE);             
	if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;                                  
	
	place(bp, asize);
	
	checkheap(v);
	return bp;
}
/*
 *  * mm_free - Free a block
 *   */
void mm_free(void *bp)
{
	if(bp == 0)
		return;

	size_t size = GET_SIZE(HDRP(bp));
	if (heap_listp == 0){
		mm_init();
	}

	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	coalesce(bp);

	checkheap(v);
}
/*
 *  * log_2: find log base 2
 *  */
static int log_2(size_t n){
	int res = 0;
	if (n == 1)
		return 0;
	n -= 1;
	while((n>>=1))
		res++;
	if (res > LSIZE){
		res = LSIZE;
	}
	return res;
}
/*
 * * allocate a free poitner to the correct list
 * */
static void allocate(void *bp, int spoint)
{
	char *bp_next = free_list[spoint];
	char *bp_prev = NULL;
	size_t size = GET_SIZE(HDRP(bp));

	while (bp_next!=NULL && GET_SIZE(HDRP(bp_next)) < size){
		bp_prev = bp_next;
		bp_next = (char*)NEXT_BLKP(bp_next);
	}

	PUT(bp, (unsigned int)(bp_next));
	PUT((char *)bp + WSIZE, (unsigned int) bp_prev);

	if (bp_prev != NULL){
		PUT((char *)bp_prev, (unsigned int) bp);
	}
	else
		free_list[spoint] = bp;

	if (bp_next != NULL){
		PUT((char *)bp_next + WSIZE, (unsigned int)(bp));
	}
}
/*
 *  *merge - connect two blocks at freeing.
 *  */
static void merge(char *bp_merge){
	char *bp_prev = 0;
	char *bp_next = 0;
	bp_next = (char *)(NEXT_BLKP(bp_merge));
	bp_prev = (char *)(PREV_BLKP(bp_merge));
	if (bp_prev != NULL)
		PUT(bp_prev, (unsigned int)(bp_next));
	else
		free_list[log_2(GET_SIZE(HDRP(bp_merge)))] = bp_next;
	if (bp_next != NULL)
		PUT((char *)bp_next + WSIZE, (unsigned int)(bp_prev));
}
/*
 *  * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 *   */
static void *coalesce(void *bp)
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_PHY_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_PHY_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));
	char *bp_merge = 0;
	char *bp_merge2 = 0;

	if (prev_alloc && !next_alloc) {      /* Case 3 */
		bp_merge = NEXT_PHY_BLKP(bp);
		merge(bp_merge);
		size += GET_SIZE(HDRP(bp_merge));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	}

	else if (!prev_alloc && next_alloc) {      /* Case 2 */
		bp_merge = PREV_PHY_BLKP(bp);
		merge(bp_merge);

		size += GET_SIZE(HDRP(bp_merge));
		bp = bp_merge;
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	}

	else if (!prev_alloc && !next_alloc){                                     /* Case 4 */
		bp_merge = PREV_PHY_BLKP(bp);
		merge(bp_merge);
		
		bp_merge2 = NEXT_PHY_BLKP(bp);
		merge(bp_merge2);
		
		size += GET_SIZE(HDRP(bp_merge)) + GET_SIZE(FTRP(bp_merge2));
		bp = bp_merge;
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	}
	/* Case 1*/
	allocate(bp, log_2(size));
	return bp;
}

/*
 *  * mm_realloc - implementation of realloc
 *   */
void *mm_realloc(void *ptr, size_t size)
{
	size_t oldsize;
	size_t asize;
	size_t nextsize;
	size_t extendsize;
	void *newptr;
	void *next_ptr;

	/* If size == 0 then this is just free, and we return NULL. */
	if(size == 0) {
		mm_free(ptr);
		return 0;
	}

	/* If oldptr is NULL, then this is just malloc. */
	if(ptr == NULL) {
		return mm_malloc(size);
	}
	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= DSIZE)                                          
		asize = 2*DSIZE;                                        
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE); 
	/*So we need to allocate asize bits*/
	oldsize = GET_SIZE(HDRP(ptr));
	next_ptr = NEXT_PHY_BLKP(ptr);
	nextsize = GET_SIZE(HDRP(next_ptr));

	if (oldsize > asize + 2*DSIZE){
		PUT(HDRP(ptr), PACK(asize, 1));
		PUT(FTRP(ptr), PACK(asize, 1));
		next_ptr = NEXT_PHY_BLKP(ptr);
		PUT(HDRP(next_ptr), PACK(oldsize-asize, 0));
		PUT(FTRP(next_ptr), PACK(oldsize-asize, 0));
		coalesce(next_ptr);
		return ptr;
	}

	if (oldsize == asize)
		return ptr;

	if (GET_SIZE(HDRP(next_ptr)) == 0){
		extendsize = MAX(asize,CHUNKSIZE);
		if (extend_heap(extendsize/WSIZE) == NULL)
			return NULL;                            
	}

	if ((GET_ALLOC(HDRP(next_ptr))) || ((nextsize+oldsize) < asize)){
		newptr = mm_malloc(size);

		/* If realloc() fails the original block is left untouched  */
		if(!newptr) {
			return 0;
		}
		/* Copy the old data. */
		memcpy(newptr, ptr, oldsize);
		/* Free the old block. */
		mm_free(ptr);
		return newptr;
	}
	place(next_ptr, asize - oldsize);
	PUT(HDRP(ptr), PACK(asize, 1));
	PUT(FTRP(ptr), PACK(asize, 1));
	return ptr;
}

/* 
 *  * mm_checkheap - Check the heap for consistency 
 *   */
void mm_checkheap(int verbose) 
{
	char *bp = heap_listp;

	if (verbose)
		printf("Heap (0x%x):\n", (unsigned) heap_listp);

	if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
		printf("Bad prologue header\n");
	checkblock(heap_listp);

	for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_PHY_BLKP(bp)) {
		if (verbose) 
			printblock(bp);
		checkblock(bp);
	}
	if (verbose)
		printblock(bp);
	if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
		printf("Bad epilogue header\n");
}


/*
 *  * The remaining routines are internal helper routines
 *   */

/*
 *  * extend_heap - Extend heap with free block and return its block pointer
 *   */
static void *extend_heap(size_t words)
{
	char *bp;
	size_t size;

	/* Allocate an even number of words to maintain alignment */
	size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
	if ((long)(bp = mem_sbrk(size)) == -1)
		return NULL;                                       
	/* Initialize free block header/footer and the epilogue header */
	PUT(HDRP(bp), PACK(size, 0));         /* Free block header */   
	PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */ 
	PUT(HDRP(NEXT_PHY_BLKP(bp)), PACK(0, 1));        /* New epilogue header */

	/* Coalesce if the previous block was free */
	return coalesce(bp);                       
}
/*
 *  * place - Place block of asize bytes at start of free block bp
 *   *         and split if remainder would be at least minimum block size
 *    */
static void place(void *bp, size_t asize)
{
	size_t csize = GET_SIZE(HDRP(bp));
	char *bp_split;
	char *bp_next;
	char *bp_prev;

	if ((csize - asize) >= (2*DSIZE)) {
		bp_next = (char *)(NEXT_BLKP(bp));
		bp_prev = (char *)(PREV_BLKP(bp));
		if (bp_prev != NULL)
			PUT(bp_prev, (unsigned int)(bp_next));
		else
			free_list[log_2(csize)] = bp_next;
		if (bp_next != NULL)
			PUT(bp_next + WSIZE, (unsigned int)(bp_prev));
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));

		bp_split = NEXT_PHY_BLKP(bp);
		PUT(HDRP(bp_split), PACK(csize-asize, 0));
		PUT(FTRP(bp_split), PACK(csize-asize, 0));
		coalesce(bp_split);
	}
	else {
		bp_next = (char *)(NEXT_BLKP(bp));
		bp_prev = (char *)(PREV_BLKP(bp));
		if (bp_prev != NULL)
			PUT(bp_prev, (unsigned int)(bp_next));
		else
			free_list[log_2(csize)] = bp_next;
		if (bp_next != NULL)
			PUT(bp_next + WSIZE, (unsigned int)(bp_prev));
		PUT(HDRP(bp), PACK(csize, 1));
		PUT(FTRP(bp), PACK(csize, 1));
	}
}

/*
 *  * find_fit - Find a fit for a block with asize bytes
 *   */
static void *find_fit(size_t asize)
{
	/* First fit search */
	void *bp;
	int i;
	int spoint = log_2(asize);
	for (i = spoint; i < LSIZE; i++){
		for (bp = free_list[i]; bp != NULL; bp = (char *)(NEXT_BLKP(bp))) {
			if ((asize <= GET_SIZE(HDRP(bp)))) {
				return bp;
			}
		}
	}
	return NULL; /* No fit */
}

void printblock(void *bp) 
{
	int hsize, halloc, fsize, falloc;
	unsigned next = 0;
	unsigned prev = 0;

	hsize = GET_SIZE(HDRP(bp));
	halloc = GET_ALLOC(HDRP(bp));
	fsize = GET_SIZE(FTRP(bp));
	falloc = GET_ALLOC(FTRP(bp));  

	if (hsize == 0) {
		printf("0x%x: EOL\n", (unsigned) bp);
		return;
	}

	if (hsize >= 2*DSIZE) {
		next = NEXT_BLKP(bp);
		prev = PREV_BLKP(bp);}
		printf("0x%x: header: [%d:%c] next_blk [%x] prev_blk [%x] footer: [%d:%c]\n", 
				(unsigned) bp,
				hsize, (halloc ? 'a' : 'f'),
				next,
				prev,
				fsize, (falloc ? 'a' : 'f'));

}

static void checkblock(void *bp) 
{
	if ((int)bp % 8)
		printf("Error: %p is not doubleword aligned\n", bp);
	if (GET(HDRP(bp)) != GET(FTRP(bp)))
		printf("Error: header does not match footer\n");
}


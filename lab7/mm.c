/*
 * mm.c - The fast and memory-efficient malloc package.
 * 
 * In this approach, a block is allocated by mm_malloc which is combined
 * with find_fit, place, etc. A block will not be overlap. And each block
 * has his header and footer. Free block can be coalesced and reused. Realloc
 * mainly use mm_malloc and mm_free.
 * 
 * Free block:
 * We have a free block list, it has header and footer. And we view each
 * free block as a node, where we store the offset and the next free block's
 * offset. Each time we find fit, we just need to begin from free list's
 * header to the footer, when we got a zero offset.
 * 
 * Allocated block:
 * Each allocated block has header and footer, which store the size and
 * information of allocation. In this way, it's easy for us to track the
 * pre and next block.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* Head of heap list */
static void *heap_listp;
/* Header and footer of free block list */
static void *free_listh;
static void *free_listf; 

/* max operation */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Basic constants and macros */
#define WSIZE 4  /* Word and header/footer size (bytes) */
#define DSIZE 8  /* Double word size (bytes) */
#define CHUNKSIZE (1 << 9) /* Extend heap by this amount (bytes) */

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks*/
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE((HDRP(bp))))
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))

/* Offset of free block */
#define GET_OFFSET(bp)  (bp - heap_listp)
#define GET_ADDR(offset)  (heap_listp + offset) 

/* Function declaration */
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *next_free(void *bp);
static void *delete_free(void *bp);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
	if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
		return -1;

	PUT(heap_listp, 0);  /* Alignment padding */
	PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));  /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));  /* Prologue footer */
	PUT(heap_listp + (3*WSIZE), PACK(0, 1));  /* Epilogue headder */

	/* List */
	heap_listp += (2*WSIZE);
	free_listh = NULL;
	free_listf = NULL;

	/* Extend the empty heap with a free block of CHUNKSIZE bytes */
	if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
		return -1;

    return 0;
}

/* 
 * extend_heap - extend_heap of words size. 
 */
static void *extend_heap(size_t words)
{
    char *bp;
	size_t size;
	
	/* Allocate an even number of words to maintain alignment */
	size = (words%2) ? (words+1)*WSIZE : words*WSIZE;
	 
	if ((long)(bp = mem_sbrk(size)) == -1)
		return NULL;

	/* Initialize free block header/footer and the epilogue headers */
	PUT(HDRP(bp), PACK(size, 0));  /* Free block header */
	PUT(FTRP(bp), PACK(size, 0));  /* Free block footer */
   	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  /* New epilogue header */

	/* Coalesce if the previous block was free */
	return coalesce(bp);

}

/* 
 * find_fit - find suitable free block for reuse. 
 */
static void *find_fit(size_t asize)
{
	/* First-fit search */
    void *bp = free_listh;

	while (bp)
	{	
		if ((asize <= GET_SIZE(HDRP(bp)))) 
			return bp;
		bp = next_free(bp);
	}
	
    /* No fit */
	return NULL;
}

/* 
 * place - set the free block to allocated block. 
 */
static void *place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
	
	/* Free block too large */
	if ((csize - asize) >= (2*DSIZE))
	{
		PUT(HDRP(bp), PACK(csize - asize, 0)); 
		PUT(FTRP(bp), PACK(csize - asize, 0));
		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
	}
	/* Directly use free block */
	else{
		PUT(HDRP(bp), PACK(csize, 1));
		PUT(FTRP(bp), PACK(csize, 1));
		delete_free(bp);
	}
	return bp;
}

/*
 * next_free - get next free block
 */
static void *next_free(void *bp)
{
	/* Get next offset */
	size_t offset = GET(bp + 4);
	/* Offset valid */
	if (offset > 0) 
		return GET_ADDR(offset);
	else 
		return NULL;
}

/*
 * this_free - get this free block
 */
static void *this_free(void *bp)
{
	/* Get next offset */
    size_t offset = GET(bp);
	/* Offset valid */
    if (offset > 0)
        return GET_ADDR(offset);
    else
        return NULL;
}

/*
 * add_free - add free block to free list
 */
static void *add_free(void *bp)
{
	/* Add the block to the header of the list*/
	if (free_listh != NULL)
	{
		PUT(bp, 0);
        PUT(bp + 4, GET_OFFSET(free_listh));
		PUT(free_listh, GET_OFFSET(bp));
        free_listh = bp;    
	}
	else{
        free_listh = bp;
        free_listf = bp;
        PUT(bp, 0);
        PUT(bp + 4, 0);
    }
	return bp;
}

/*
 * delete_free - delete the block from free list
 */
static void *delete_free(void *bp)
{
	void *this_freep = this_free(bp);
	void *next_freep = next_free(bp);
	
	/* Header */
	if (this_freep){
		PUT(this_freep + 4, GET(bp + 4));
	}
	else{
		free_listh = next_freep;
	}
	
	/* Footer */
	if (next_freep){
		PUT(next_freep, GET(bp)); 
	}
	else{
		free_listf = this_freep;
	}
	return bp;
}


/* 
 * mm_malloc - allocate a block by incrementing the brk pointer
 * always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;	    /* Adjusted block size */
	size_t extendsize; 	/* Amount to extend heap if no fit */
	char *bp;
	
	/* Ignore spurious requests */
	if (size == 0) 
	  return NULL;

	/* Adjust block size to include overhead and alignment reqs. */
	if (size <= DSIZE) 
		asize = 2 * DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

	/* Search the free list for a fit */
	if ((bp = find_fit(asize)) != NULL){
		bp = place(bp, asize);
		return bp;
	}
   	
	/* No fit found. Get more memory and place the block */
	extendsize = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
		return NULL;

	bp = place(bp, asize);
	return bp;
}

/*
 * mm_free - freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
	
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	coalesce(bp);
}

/*
 * coalesce - coalesce near free block.
 */
static void *coalesce(void *bp)
{
    void *prev = PREV_BLKP(bp);
	void *next = NEXT_BLKP(bp);
	size_t prev_alloc = GET_ALLOC(HDRP(prev));
	size_t next_alloc = GET_ALLOC(HDRP(next));
	size_t size = GET_SIZE(HDRP(bp));
	
	/* Case 1 */
	if (prev_alloc && next_alloc){
		add_free(bp);
	}

	/* Case 2 */ 
	else if (prev_alloc && !next_alloc){
		size += GET_SIZE(HDRP(next));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
		add_free(bp);
		delete_free(next);
	}

	/* Case 3 */ 
	else if (!prev_alloc && next_alloc){
		size += GET_SIZE(HDRP(prev));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(prev), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}

	/* Case 4 */ 
	else {
		size += GET_SIZE(HDRP(prev)) + GET_SIZE(HDRP(next));
		PUT(HDRP(prev), PACK(size, 0));
		PUT(FTRP(next), PACK(size, 0));
		bp = PREV_BLKP(bp);
		delete_free(next);
	}

	return bp;
}

/*
 * mm_realloc - implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *bp, size_t size)
{
	/* Free is size equals zero */
    if (size == 0){
		mm_free(bp);
		return bp;
	}
	
	/* The actual size iterms of alignment */
	size_t asize;
	
	if (size <= DSIZE)
		asize = 2*DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);

	if (bp == NULL){
		return mm_malloc(asize);
	}	
	
	else {
		size_t csize = GET_SIZE(HDRP(bp));
		/* csize is larger */
		if (csize >= asize)  
			return bp;
		
		/* csize is litter */
		else {
			void *next = NEXT_BLKP(bp);
			/* Next block is free */
			if (!GET_ALLOC(HDRP(next)))
			{
				size_t next_size = GET_SIZE(HDRP(next));
				if (csize + next_size >= asize)
				{
					size_t size = csize + next_size;
					PUT(HDRP(bp), PACK(size, 1));
					PUT(FTRP(bp), PACK(size, 1));
					delete_free(next);
					return bp;
				}
			}
			
			/* malloc new block */
			void *new_bp = mm_malloc(asize);
			memcpy(new_bp, bp, csize - DSIZE);
			mm_free(bp);
			return new_bp;
		}	
	}
}

/*
 * mm_check - check the correctness of code
 */
int mm_check(void)
{
	void *bp = free_listh;
    void *ptr = heap_listp;
    int count1 = 0;
	int count2 = 0;

	//Is every block in the free list marked as free?
	while (bp)
	{	
		if(GET_ALLOC(HDRP(bp)) != 0)
		{
			printf("Not every block in the free list is marked as free.");
			return 0;
		}
		bp = next_free(bp);
	}
	bp = free_listh;

	//Are there any contiguous free blocks that somehow escaped coalescing?
	while(bp)
	{
		if(!GET_ALLOC(HDRP(NEXT_BLKP(bp))))
		{
			printf("There are contiguous free blocks that somehow escaped coalescing.");
			return 0;
		}
		bp = next_free(bp);
	}
	bp = free_listh;

	//Is every free block actually in the free list?
	while(ptr)
	{
		if(!GET_ALLOC(HDRP(ptr)))
			count1++;
		ptr = NEXT_BLKP(ptr);
	}
	while(bp)
	{
		count2++;
		bp = next_free(bp);
	}
	if(count1 != count2)
	{
		printf("Not every free block is in the free list.");
		return 0;
	}
	bp = free_listh;

	//Do the pointers in the free list point to valid free blocks?
	while(bp)
	{
		if (!this_free(bp))
		{
			printf("Not all the pointers in the free list point to valid free blocks.");
			return 0;
		}
		bp = next_free(bp);
	}
	return 1;
}














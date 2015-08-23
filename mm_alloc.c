/**
 * @author: sg95
 * @date  : March 2015-ish
 *
 * mm_malloc implementation
 * ------------------------
 * 1. Check if there are any free blocks that can fulfil the request. If so, give that block to the request and split.
 * 2. Otherwise, create a new block. 
 * 3. Make sure to check if a linkedlist already exists... dealing with the pointers depends on size of list.
 *
 *
 * mm_free implementation
 * ----------------------
 * 1. Free was straightforward. If ptr is NULL, don't do anything. 
 * 2. Otherwise, find the block corresponding to ptr and set it to free. (I didn't have time to implement splitting)
 *
 *
 * mm_realloc implemenetation
 * --------------------------
 * 1. If the pointer is valid but the size is 0, treat it as a call to free. 
 * 2. If the pointer isn't valid, treat it as a call to malloc.
 * 3. Otherwise, create a new block using malloc.
 *    a. If that turns out to be null, the allocation failed. Return null.
 *    b. Otherwise, use memcpy to copy over the data!
 *    c. Be sure to free ptr!
 */

#include <stdint.h>
#include "mm_alloc.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/* Your final implementation should comment out this macro. */
#define MM_USE_STUBS

s_block_ptr request(size_t);
void print_block(s_block_ptr);

/* Fields for book keeping */
s_block_ptr last_block;
s_block_ptr head;

/* For debugging purposes */
void
print_list()
{
	if (head == NULL) {
		printf("\nList is empty.\n\n");
		return;
	}

	printf("\nPRINTING LIST.\n");
		
	s_block_ptr curr;
	for (curr = head; curr; curr = curr->next) { print_block(curr); }

	printf("DONE.\n\n");
}

/**
 * Given a s_block_ptr b and a request s, checks
 * to see if we could split b into two blocks as
 * to not waste space.
 */
void
split_block (s_block_ptr b, size_t s) 
{
	// Alignment, double check that this is correct w/o more casting
	intptr_t initial = (intptr_t)(b + BLOCK_SIZE + b->size);	
	int64_t align = 0;
	while ((int64_t)(initial + align) % 4) { align++; }

	// shrink b's size to fit the request
	size_t old_size = b->size;
	b->size = s;
	
	// Set up the pointers
	s_block_ptr block = (s_block_ptr) (initial + align);

	block->next = NULL;
	block->prev = NULL;
	
	if (b->next) {
		block->next = b->next;
		b->next->prev = block;
	}
	b->next = block;
	b->free = 0;

	block->prev = b;
	block->free = 1;
	block->size = (size_t) (old_size - (s + align + BLOCK_SIZE));
}

s_block_ptr
fusion (s_block_ptr b)
{
	s_block_ptr prev = b->prev;
	s_block_ptr next = b->next;

	if (prev->free && next->free) {
		

	} else if (prev->free) {
		// remove node from list	

	} else if (next->free) {
		// remove from list

	} else {
		// neither is free... don't do anything.
		return NULL;
	}
	return NULL;
}


/**
 * Given an address p, finds the block containing it.
 */
s_block_ptr 
get_block (void *p)
{
	return NULL;
}	

void print_block(s_block_ptr b)
{
	printf("BLOCK: ptr: %p next: %p prev: %p free: %d size: %d\n", 
		(void *)b, (void *) b->next, (void *)b->prev, b->free, b->size);
}


/* Extends heap for a new block and the request. Returns a ptr to the block */
s_block_ptr 
extend_heap (s_block_ptr last, size_t s)
{
	// Make sure that start of block is aligned
	intptr_t initial = (intptr_t) sbrk(0);
	int align = 4 - (uint64_t) initial % 4;	

	if (align % 4) {
		intptr_t extend = (intptr_t) sbrk(align); 
		printf("Aligning!\n");
		if (extend < 0) { printf("Error while aligning!\n"); return NULL; }
	}
 
	// Fill in heap with new block
	s_block_ptr block = sbrk(BLOCK_SIZE + s);

	// Zero out the block
	memset(block + BLOCK_SIZE, 0, 1);
	
	if (last == NULL) {
		block->next = NULL;
		block->prev = NULL;
		block->free = 0;
		block->size = s;

		head = block;
		last_block = block;
	}

	else {
		block->next = NULL;
		block->prev = last_block;
		block->prev->next = block;
		block->free = 0;
		block->size = s;

		last_block = block;
	}

	return block;
}


/**
 * Given an address, finds the block containing that data
 */
s_block_ptr
find_block (void *address)
{
	s_block_ptr curr; 

	for (curr = head; curr; curr = (s_block_ptr) curr->next) {
		void *data = (void *) ((int64_t)curr + BLOCK_SIZE);
		if (address == data) {
			return curr; 
		}
	}
	return NULL;
}


/**
 * Given a request, finds and returns the first free block that
 * can satisfy it.
 */
s_block_ptr
request(size_t request_size) 
{
	s_block_ptr curr;	

	for (curr = head; curr; curr = (s_block_ptr) curr->next) {
		if (curr->free && curr->size >= request_size) {
			return curr;
		}
	}
	return NULL;
}

/* malloc code */
void* mm_malloc(size_t size)
{
	s_block_ptr match = request(size);
	if (match) {
		memset((void*) (match + BLOCK_SIZE), 0, match->size);
		match->free = 0;
		return (void *) ((int64_t) match + BLOCK_SIZE);
	} else {
		s_block_ptr new_block = sbrk(BLOCK_SIZE + size);
	
		// if sbrk request unsuccessful return NULL
		if ((int64_t)new_block < 0) { return NULL; }

		new_block->next = NULL;
		new_block->prev = NULL;
		new_block->free = 0;
		new_block->size = size;
	
		// if this is the first block
		if (head == NULL) {
			head = new_block;
			last_block = new_block;
		} else {
			new_block->prev = last_block;
			new_block->prev->next = new_block;
			last_block = new_block;
		}

		return (void *) ((int64_t) new_block + BLOCK_SIZE);
	}
}

void* mm_realloc(void* ptr, size_t size)
{
	if (ptr && size == 0) {
		mm_free(ptr);
		return NULL;
	}

	if (!ptr) {
		return mm_malloc(size);
	}
	
	else if (size){
		s_block_ptr new_block = mm_malloc(size);

		// If space can't be found
		if (!new_block) { return NULL; }

		// If space is found
		memcpy (new_block, ptr, size);
	
		mm_free(ptr);
		return (void*) ((int64_t) new_block);
	}

	mm_free(ptr);
	return NULL;
}

void mm_free(void* ptr)
{	
	if (!ptr) { return; }
	
	s_block_ptr target = find_block(ptr);
	target->free = 1;

	return;
}




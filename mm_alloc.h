/*
 * mm_alloc.h
 *
 * Exports a clone of the interface documented in "man 3 malloc".
 */

#pragma once

#ifndef _malloc_H_
#define _malloc_H_

 /* Define the block size since the sizeof will be wrong */
#define BLOCK_SIZE 2 * sizeof(void *) + sizeof(int) + sizeof(uint32_t)

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

void* mm_malloc(size_t size);
void* mm_realloc(void* ptr, size_t size);
void mm_free(void* ptr);

typedef struct s_block *s_block_ptr;

/* block struct */
struct s_block {
    struct s_block *next;
    struct s_block *prev;
    int free;
    uint32_t size;
 };

/* Split block according to size, b must exist */
void split_block (s_block_ptr b, size_t s);

/* Try fusing block with neighbors */
s_block_ptr fusion(s_block_ptr b);

/* Get the block from addr */
s_block_ptr get_block (void *p);

/* Add a new block at the of heap,
 * return NULL if things go wrong
 */
s_block_ptr extend_heap (s_block_ptr last , size_t s);

/* Finds the best size-match possible */
s_block_ptr find_suitable_match (size_t request_size);

#ifdef __cplusplus
}
#endif

#endif

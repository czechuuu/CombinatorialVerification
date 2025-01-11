#pragma once
#include "nonrecursive/shared_sumset.h"
#include <stdlib.h>

#define POOL_BLOCK_SIZE 512

// ! POOL CLOSE MUST BE CALLED AT THE END OF THE PROGRAM
typedef struct block {
    SharedSumset* block_beginning;
    struct block* next;
} block;

static int pool_free_sumsets = 0;
static SharedSumset* pool_list_head = NULL;
static block* pool_blocks = NULL;

static inline void _pool_alloc_new_block()
{
    SharedSumset* new_block = malloc(sizeof(SharedSumset) * POOL_BLOCK_SIZE);
    if (!new_block) {
        exit(1);
    }

    for (size_t i = 0; i < POOL_BLOCK_SIZE - 1; i++) {
        new_block[i].next = new_block + i + 1;
    }
    new_block[POOL_BLOCK_SIZE - 1].next = NULL;

    pool_list_head = new_block;
    pool_free_sumsets = POOL_BLOCK_SIZE;

    block* new_block_info = malloc(sizeof(block));
    if (!new_block_info) {
        exit(1);
    }
    new_block_info->block_beginning = new_block;
    new_block_info->next = pool_blocks;
    pool_blocks = new_block_info;
}

// tries to allocate a new SharedSumset from the pool
// if the pool is full, it will allocate on the heap
static inline SharedSumset* pool_new_from_existing(Sumset const sumset)
{
    if (pool_free_sumsets == 0) {
        _pool_alloc_new_block();
    }

    SharedSumset* new_sumset = pool_list_head;
    pool_list_head = pool_list_head->next;
    pool_free_sumsets--;

    shared_sumset_initialize(new_sumset, sumset);
    return new_sumset;
}

static inline SharedSumset* pool_new_empty()
{
    if (pool_free_sumsets == 0) {
        _pool_alloc_new_block();
    }

    SharedSumset* new_sumset = pool_list_head;
    pool_list_head = pool_list_head->next;
    pool_free_sumsets--;

    return new_sumset;
}

static inline void pool_give_back(SharedSumset* shared_sumset)
{
    shared_sumset->next = pool_list_head;
    pool_list_head = shared_sumset;
    pool_free_sumsets++;
}


static inline void pool_close()
{
    block* current_block = pool_blocks;
    while (current_block) {
        block* next = current_block->next;
        free(current_block->block_beginning);
        free(current_block);
        current_block = next;
    }
}

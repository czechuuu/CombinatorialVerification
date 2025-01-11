#pragma once
#include "parallel/smart_sumset.h"
#include <stdlib.h>

#define POOL_BLOCK_SIZE 512

// ! POOL CLOSE MUST BE CALLED AT THE END OF THE PROGRAM
typedef struct block{
    SmartSumset* block_beginning;
    struct block* next;
} block;

typedef struct {
    size_t free_sumsets;
    SmartSumset* list_head;
    block* blocks_list;
} Pool;

static inline void pool_init(Pool* pool)
{
    pool->free_sumsets = 0;
    pool->list_head = NULL;
    pool->blocks_list = NULL;
}

static inline void _pool_alloc_new_block(Pool* pool){
    SmartSumset* new_block = malloc(sizeof(SmartSumset) * POOL_BLOCK_SIZE);
    if(!new_block){
        exit(1);
    }

    for (size_t i = 0; i < POOL_BLOCK_SIZE - 1; i++) {
        new_block[i].next = new_block + i + 1;
    }
    new_block[POOL_BLOCK_SIZE - 1].next = NULL;

    pool->list_head = new_block;
    pool->free_sumsets = POOL_BLOCK_SIZE; // ! copilot thinks -1

    block* new_block_info = malloc(sizeof(block));
    if(!new_block_info){
        exit(1);
    }
    new_block_info->block_beginning = new_block;
    new_block_info->next = pool->blocks_list;
    pool->blocks_list = new_block_info;
}

// tries to allocate a new SharedSumset from the pool
// if the pool is full, it will allocate on the heap
static inline SmartSumset* pool_new_from_existing(Pool* pool, Sumset const sumset)
{
    if (pool->free_sumsets == 0) { 
        _pool_alloc_new_block(pool);
    }

    SmartSumset* new_sumset = pool->list_head;
    pool->list_head = pool->list_head->next;
    pool->free_sumsets--;

    new_sumset->val = sumset;
    new_sumset->ref_count = 1;
    new_sumset->parent = NULL;
    new_sumset->next = NULL;
    
    return new_sumset;
}

static inline SmartSumset* pool_new_empty(Pool* pool){
    if (pool->free_sumsets == 0) { 
        _pool_alloc_new_block(pool);
    }

    SmartSumset* new_sumset = pool->list_head;
    pool->list_head = pool->list_head->next;
    pool->free_sumsets--;

    new_sumset->ref_count = 1;
    // val will be overwritten when we use it as a store for the addition
    // parent will be set
    // next is irrelevant

    return new_sumset;
}

static inline void _pool_give_back(Pool* pool, SmartSumset* smart_sumset)
{
    smart_sumset->next = pool->list_head;
    pool->list_head = smart_sumset;
    pool->free_sumsets++;
}

static inline void pool_release(Pool* pool, SmartSumset* smart_sumset)
{
    // -1 because it returns the value before the decrement
    if (smart_sumset_dec_ref(smart_sumset) - 1 == 0) {
        if(smart_sumset->parent){
            pool_release(pool, smart_sumset->parent); // TODO iterative?
        }
        _pool_give_back(pool, smart_sumset);
    }
}

static inline void pool_close(Pool* pool)
{
    block* current_block = pool->blocks_list;
    while(current_block){
        block* next = current_block->next;
        free(current_block->block_beginning);
        free(current_block);
        current_block = next;
    }
}

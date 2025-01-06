#pragma once
#include "nonrecursive/shared_sumset.h"
#include <stdlib.h>

#define POOL_SIZE 16384

static SharedSumset pool[POOL_SIZE]; // a free sumset has ref_count = -1
static size_t pool_free_sumsets = POOL_SIZE;
static int pool_free_index = 0;

// must be called before any pool_new
static inline void pool_init()
{
    for (size_t i = 0; i < POOL_SIZE; ++i) {
        pool[i].ref_count = -1;
    }
}

// allocates a new SharedSumset on the heap using malloc
static inline SharedSumset* _pool_new_heap(Sumset const sumset)
{
    SharedSumset* shared_sumset = (SharedSumset*)malloc(sizeof(SharedSumset));
    if (!shared_sumset) {
        exit(1);
    }

    shared_sumset_initialize(shared_sumset, sumset);
    return shared_sumset;
}

// tries to allocate a new SharedSumset from the pool
// if the pool is full, it will allocate on the heap
static inline SharedSumset* pool_new(Sumset const sumset)
{
    if (pool_free_sumsets == 0) { 
        return _pool_new_heap(sumset);
    }

    // cyclic find free index
    while (pool[pool_free_index].ref_count != -1) {
        pool_free_index = (pool_free_index + 1) % POOL_SIZE;
    }

    SharedSumset* shared_sumset = pool + pool_free_index;
    shared_sumset_initialize(shared_sumset, sumset);

    pool_free_index = (pool_free_index + 1) % POOL_SIZE;
    pool_free_sumsets--;
    return shared_sumset;
}

// frees a SharedSumset, either by setting it as free in the pool
// or if its on the heap by calling free
static inline void _pool_free(SharedSumset* shared_sumset)
{
    ssize_t index = shared_sumset - pool;
    if (index < 0 || index >= POOL_SIZE) {
        free(shared_sumset);
    } else {
        pool_free_sumsets++;
        shared_sumset->ref_count = -1;
    }
}

static inline void pool_release(SharedSumset* shared_sumset)
{
    _shared_sumset_dec_ref(shared_sumset);
    if (shared_sumset->ref_count == 0) {
        if(shared_sumset->parent){
            pool_release(shared_sumset->parent); // TODO iterative?
        }
        _pool_free(shared_sumset);
    }
}

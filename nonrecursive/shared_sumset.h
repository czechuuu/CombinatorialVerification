#pragma once
#include "common/io.h"
#include "common/sumset.h"
#include <stdlib.h>

typedef struct SharedSumset {
    Sumset* ptr; 
    size_t ref_count;
    struct SharedSumset* parent;
} SharedSumset;

static inline void shared_sumset_inc_ref(SharedSumset* shared_sumset)
{
    shared_sumset->ref_count++;
}

static inline void shared_sumset_dec_ref(SharedSumset* shared_sumset)
{
    shared_sumset->ref_count--;
    if (shared_sumset->ref_count == 0)
    {
        free(shared_sumset->ptr);
        free(shared_sumset);
        if(shared_sumset->parent){
            shared_sumset_dec_ref(shared_sumset->parent); // TODO maybe change this to iterative
        }
    }
}

static inline SharedSumset* shared_sumset_init(Sumset const sumset)
{
    SharedSumset* shared_sumset = malloc(sizeof(SharedSumset));
    if(!shared_sumset){
        exit(1);
    }

    shared_sumset->ptr = malloc(sizeof(Sumset));
    if(!shared_sumset->ptr){
        free(shared_sumset);
        exit(1);
    }

    *shared_sumset->ptr = sumset;
    shared_sumset->ref_count = 1;
    shared_sumset->parent = NULL;
    return shared_sumset;
}

static inline SharedSumset* shared_sumset_init_parent(Sumset const sumset, SharedSumset* parent)
{
    SharedSumset* shared_sumset = shared_sumset_init(sumset);
    shared_sumset->parent = parent;
    if(parent != NULL){
        shared_sumset_inc_ref(parent);
    }
    return shared_sumset;
}


static inline Sumset* shared_sumset_get(SharedSumset* shared_sumset)
{
    return shared_sumset->ptr;
}
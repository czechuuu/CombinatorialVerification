#pragma once
#include "common/io.h"
#include "common/sumset.h"
#include <stdlib.h>
#include <sys/types.h>

typedef struct SharedSumset {
    Sumset val; 
    ssize_t ref_count;
    struct SharedSumset* parent;
    struct SharedSumset* next; // for pool list
} SharedSumset;

static inline ssize_t shared_sumset_get_ref_count(SharedSumset* shared_sumset)
{
    return shared_sumset->ref_count;
}

static inline void shared_sumset_inc_ref(SharedSumset* shared_sumset)
{
    shared_sumset->ref_count++;
}

// ! DOES NOT CALL FREE
static inline void _shared_sumset_dec_ref(SharedSumset* shared_sumset)
{
    shared_sumset->ref_count--;
}

static inline void shared_sumset_initialize(SharedSumset* shared_sumset, Sumset const sumset)
{
    shared_sumset->val = sumset;
    shared_sumset->ref_count = 1;
    shared_sumset->parent = NULL; // can probably remove for minor speedup
    shared_sumset->next = NULL;
}

static inline void shared_sumset_set_parent(SharedSumset* shared_sumset, SharedSumset* parent)
{
    shared_sumset->parent = parent;
    if(parent){
        shared_sumset_inc_ref(parent);
    }
}

static inline Sumset shared_sumset_get(SharedSumset* shared_sumset)
{
    return shared_sumset->val;
}

static inline Sumset* shared_sumset_get_ptr(SharedSumset* shared_sumset)
{
    return &shared_sumset->val;
}

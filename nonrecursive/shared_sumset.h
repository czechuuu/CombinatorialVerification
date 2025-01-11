#pragma once
#include "common/io.h"
#include "common/sumset.h"
#include <stdlib.h>
#include <sys/types.h>

typedef struct SharedSumset {
    Sumset val;
    struct SharedSumset* next; // for pool list
} SharedSumset;


static inline void shared_sumset_initialize(SharedSumset* shared_sumset, Sumset const sumset)
{
    shared_sumset->val = sumset;
    shared_sumset->next = NULL;
}

static inline Sumset shared_sumset_get(SharedSumset* shared_sumset)
{
    return shared_sumset->val;
}

static inline Sumset* shared_sumset_get_ptr(SharedSumset* shared_sumset)
{
    return &shared_sumset->val;
}
#pragma once
#include "common/io.h"
#include "common/sumset.h"

typedef struct {
    Sumset* ptr;
    size_t ref_count;
} SharedSumset;

// Constructs a new SharedSumset from the given Sumset
SharedSumset* shared_sumset_init(Sumset sumset);

// Increments the reference count of the SharedSumset
void shared_sumset_inc_ref(SharedSumset* shared_sumset);

// Decrements the reference count of the SharedSumset.
// If count reaches 0, the SharedSumset is freed.
void shared_sumset_dec_ref(SharedSumset* shared_sumset);

Sumset* shared_sumset_get(SharedSumset* sumset);
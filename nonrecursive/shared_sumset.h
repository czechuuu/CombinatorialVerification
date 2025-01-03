#pragma once
#include "common/io.h"
#include "common/sumset.h"

typedef struct SharedSumset {
    Sumset* ptr; 
    size_t ref_count;
    // in order to be able to recreate the multiset result 
    // we need to preserve a reference to the sumset from which we were created
    // so as to keep it alive
    struct SharedSumset* child; // ? parent would be a better name probably
} SharedSumset;

// Constructs a new SharedSumset from the given Sumset
// Sets child to NULL, if the SharedSumset has a child shared_sumset_init_child should be used
SharedSumset* shared_sumset_init(Sumset sumset);

// Constructs a new SharedSumset from the given Sumset and sets the child of the SharedSumset
SharedSumset* shared_sumset_init_child(Sumset sumset, SharedSumset* child);

// Increments the reference count of the SharedSumset
void shared_sumset_inc_ref(SharedSumset* shared_sumset);

// Decrements the reference count of the SharedSumset.
// If count reaches 0, the SharedSumset is freed.
void shared_sumset_dec_ref(SharedSumset* shared_sumset);

Sumset* shared_sumset_get(SharedSumset* shared_sumset);
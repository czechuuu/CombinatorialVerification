#pragma once
#include "common/io.h"
#include "common/sumset.h"
#include <stdatomic.h>

typedef struct SmartSumset {
    Sumset val;
    atomic_size_t ref_count;
    struct SmartSumset* parent;
    struct SmartSumset* next; // for pool list
} SmartSumset;

static inline size_t smart_sumset_get_ref_count(SmartSumset* smart_sumset)
{
    return atomic_load(&smart_sumset->ref_count);
}

static inline size_t smart_sumset_inc_ref(SmartSumset* smart_sumset)
{
    return atomic_fetch_add(&smart_sumset->ref_count, 1);
}

static inline size_t smart_sumset_dec_ref(SmartSumset* smart_sumset)
{
    return atomic_fetch_sub(&smart_sumset->ref_count, 1);
}

static inline Sumset smart_sumset_get(SmartSumset* smart_sumset)
{
    return smart_sumset->val;
}

static inline Sumset* smart_sumset_get_ptr(SmartSumset* smart_sumset)
{
    return &smart_sumset->val;
}

static inline void smart_sumset_set_parent(SmartSumset* smart_sumset, SmartSumset* parent)
{
    smart_sumset->parent = parent;
    if (parent) {
        smart_sumset_inc_ref(parent);
    }
}

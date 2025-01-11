#pragma once
#include "common/io.h"
#include "common/sumset.h"
#include "parallel/pool.h"
#include "parallel/safe_stack.h"
#include "parallel/smart_sumset.h"

typedef struct {
    InputData* input_data;
    SafeStack* stack;
    Solution best_solution;
} WorkerData;

// number of unconditional pushes to the stack, before considering recursion
#define INITIAL_STACK_PUSHES 5

static inline void recursive_solve(Sumset* a, Sumset* b, WorkerData* data)
{
    if (a->sum > b->sum) {
        return recursive_solve(b, a, data);
    }

    if (is_sumset_intersection_trivial(a, b)) {
        for (size_t i = a->last; i <= data->input_data->d; i++) {
            if (!does_sumset_contain(b, i)) {
                Sumset a_with_i;
                sumset_add(&a_with_i, a, i);
                recursive_solve(&a_with_i, b, data);
            }
        }
    } else if (a->sum == b->sum && get_sumset_intersection_size(a, b) == 2) {
        // saves to a local solution
        if(a->sum > data->best_solution.sum)
            solution_build(&data->best_solution, data->input_data, a, b);
    }
}

static inline void populate_stack(SmartSumset* smart_a, SmartSumset* smart_b, SafeStack* stack, Pool* pool, WorkerData* data)
{
    if (smart_sumset_get(smart_a).sum > smart_sumset_get(smart_b).sum) {
        return populate_stack(smart_b, smart_a, stack, pool, data);
    }

    Sumset* a = smart_sumset_get_ptr(smart_a);
    Sumset* b = smart_sumset_get_ptr(smart_b);

    if (is_sumset_intersection_trivial(a, b)) {
        for (size_t i = a->last; i <= data->input_data->d; i++) {
            if (!does_sumset_contain(b, i)) {
                SmartSumset* a_with_i = pool_new_empty(pool);
                sumset_add(smart_sumset_get_ptr(a_with_i), a, i);
                smart_sumset_set_parent(a_with_i, smart_a);
                smart_sumset_inc_ref(smart_b); // before pushing to the stack
                safe_stack_push(stack, safe_stack_pair_construct(a_with_i, smart_b));
            }
        }
    } else if (a->sum == b->sum && get_sumset_intersection_size(a, b) == 2) {
        // saves to a local solution
        if(a->sum > data->best_solution.sum)
            solution_build(&data->best_solution, data->input_data, a, b);
    }
}

bool is_stack_small(WorkerData* data)
{
    return atomic_load(&data->stack->size) <= data->input_data->t * 2;
}

void* worker_thread(void* arg)
{
    WorkerData* data = (WorkerData*)arg;

    Pool pool;
    pool_init(&pool);

    int iteration = 0;
    while (true) {
        SafeStackPair pair = safe_stack_pop(data->stack);
        if (pair.a == NULL) {
            break; // end of the program
        }

        if (is_stack_small(data) || iteration < INITIAL_STACK_PUSHES) {
            populate_stack(pair.a, pair.b, data->stack, &pool, data);
        } else {
            recursive_solve(smart_sumset_get_ptr(pair.a), smart_sumset_get_ptr(pair.b), data);
        }

        pool_release(&pool, pair.a);
        pool_release(&pool, pair.b);
        iteration++;
    }

    pool_close(&pool);

    return NULL;
}
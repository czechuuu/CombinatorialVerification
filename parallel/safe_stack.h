#pragma once
#include "common/sumset.h"
#include "common/io.h"
#include "parallel/smart_sumset.h"
#include "common/err.h"
#include <stdatomic.h>
#include <pthread.h>
#include <stdlib.h>

// functions with _unsafe prefix are not thread safe
// and mutex must be taken before calling them

#define INITIAL_CAPACITY 512

typedef struct SafeStackPair {
    SmartSumset* a;
    SmartSumset* b;
} SafeStackPair;

static inline SafeStackPair safe_stack_pair_construct(SmartSumset* a, SmartSumset* b)
{
    SafeStackPair pair = {a, b};
    return pair;
}

typedef struct SafeStack {
    SafeStackPair* stack;
    atomic_size_t size; // atomic to check size without taking the mutex
    size_t capacity;
    pthread_mutex_t mutex;
    pthread_cond_t delay;
    size_t threads_waiting;
    size_t max_threads;
    bool done;
} SafeStack;

static inline size_t safe_stack_size(SafeStack* stack)
{
    return atomic_load(&stack->size);
}

// Initialize a safe stack.
// Must be called once before using the stack, and from a single thread only.
static inline void safe_stack_init(SafeStack* stack, InputData* input_data)
{
    atomic_store(&stack->size, 0);
    stack->capacity = INITIAL_CAPACITY;
    stack->stack = malloc(sizeof(SafeStackPair) * stack->capacity);
    if (!stack->stack) {
        exit(1);
    }
    ASSERT_ZERO(pthread_mutex_init(&stack->mutex, NULL));
    ASSERT_ZERO(pthread_cond_init(&stack->delay, NULL));
    stack->threads_waiting = 0;
    stack->max_threads = input_data->t;
    stack->done = false;
}

// ! Does not take the mutex.
// Mutex must be taken before calling this.
static inline void _unsafe_safe_stack_resize(SafeStack* stack)
{
    if (atomic_load(&stack->size) == stack->capacity) {
        stack->capacity *= 2;
        stack->stack = realloc(stack->stack, sizeof(SmartSumset) * stack->capacity);
        if (!stack->stack) {
            exit(1);
        }
    }
}

static inline void _unsafe_safe_stack_push(SafeStack* stack, SafeStackPair pair)
{
    _unsafe_safe_stack_resize(stack); // ensure there is enough space
    stack->stack[atomic_fetch_add(&stack->size, 1)] = pair; // add to the stack
}

static inline void safe_stack_push(SafeStack* stack, SafeStackPair pair)
{
    ASSERT_ZERO(pthread_mutex_lock(&stack->mutex));

    _unsafe_safe_stack_push(stack, pair); // add to the stack
    ASSERT_ZERO(pthread_cond_signal(&stack->delay)); // and wake up someone waiting for it

    ASSERT_ZERO(pthread_mutex_unlock(&stack->mutex));
}

// Blocks until a pair is available to pop, or the program is done.
static inline SafeStackPair safe_stack_pop(SafeStack* stack){
    ASSERT_ZERO(pthread_mutex_lock(&stack->mutex));

    if(atomic_load(&stack->size) == 0){ // can't pop
        stack->threads_waiting++;
        if(stack->threads_waiting == stack->max_threads){ // everything is done, end the program
            stack->done = true;
            ASSERT_ZERO(pthread_cond_broadcast(&stack->delay));
        } else{
            while(atomic_load(&stack->size) == 0 && !stack->done){ // wait for something to be pushed
                ASSERT_ZERO(pthread_cond_wait(&stack->delay, &stack->mutex));
            }
        } 
        stack->threads_waiting--; // fuck me

        if(stack->done){ // if we are done, return an empty pair
            // either we begun the cascade, or were woken up
            // either way we need to finish
            ASSERT_ZERO(pthread_mutex_unlock(&stack->mutex));
            return safe_stack_pair_construct(NULL, NULL);
        }
    }

    SafeStackPair pair = stack->stack[atomic_fetch_sub(&stack->size, 1) - 1]; // pop from the stack
    ASSERT_ZERO(pthread_mutex_unlock(&stack->mutex));
    return pair;
}

static inline void safe_stack_destroy(SafeStack* stack)
{
    ASSERT_ZERO(pthread_mutex_destroy(&stack->mutex));
    ASSERT_ZERO(pthread_cond_destroy(&stack->delay));
    free(stack->stack);
}

typedef struct SafeStackPairBatch {
    SafeStackPair pairs[MAX_D];
    size_t size;
    size_t capacity;
} SafeStackPairBatch;

static inline void safe_stack_pair_batch_init(SafeStackPairBatch* batch)
{
    batch->size = 0;
    batch->capacity = MAX_D;
}

static inline void safe_stack_pair_batch_add(SafeStackPairBatch* batch, SafeStackPair pair)
{
    // what if the batch is full?
    if (batch->size == batch->capacity) {
        syserr("Batch full");
    }
    batch->pairs[batch->size++] = pair;
}

static inline void safe_stack_pair_batch_submit(SafeStack* stack, SafeStackPairBatch* batch)
{
    ASSERT_ZERO(pthread_mutex_lock(&stack->mutex));
    for(int i = 0; i < batch->size; i++){
        _unsafe_safe_stack_push(stack, batch->pairs[i]);
    }
    batch->size = 0;
    ASSERT_ZERO(pthread_cond_broadcast(&stack->delay));
    ASSERT_ZERO(pthread_mutex_unlock(&stack->mutex));
}







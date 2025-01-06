#include "common/sumset.h"
#include "nonrecursive/shared_sumset.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 1

typedef struct {
    SharedSumset* a;
    SharedSumset* b;
} Pair;

static inline Pair pair_construct(SharedSumset* a, SharedSumset* b)
{
    Pair pair = {a, b};
    return pair;
}

typedef struct {
    Pair* pairs;
    size_t size;
    size_t capacity;
} PairStack;

static inline void pair_stack_init(PairStack* stack)
{
    stack->size = 0;
    stack->capacity = INITIAL_CAPACITY;
    stack->pairs = malloc(sizeof(Pair) * stack->capacity);
    if(!stack->pairs){
        exit(1);
    }
}

static inline void _pair_stack_resize(PairStack* stack)
{
    if(stack->size == stack->capacity){
        stack->capacity *= 2;
        stack->pairs = realloc(stack->pairs, sizeof(Pair) * stack->capacity);
        if(!stack->pairs){
            exit(1);
        }
    }
}

static inline void pair_stack_push(PairStack* stack, Pair pair)
{
    _pair_stack_resize(stack);
    stack->pairs[stack->size++] = pair;
}

static inline Pair pair_stack_pop(PairStack* stack)
{
    return stack->pairs[--stack->size];
}

static inline bool pair_stack_empty(PairStack* stack)
{
    return stack->size == 0;
}

static inline void pair_stack_destroy(PairStack* stack)
{
    free(stack->pairs);
}

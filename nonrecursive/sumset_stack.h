#include "common/sumset.h"
#include "nonrecursive/shared_sumset.h"

typedef struct {
    SharedSumset* a;
    SharedSumset* b;
} Pair;

Pair pair_construct(SharedSumset* a, SharedSumset* b);

typedef struct {
    Pair* pairs;
    size_t size;
    size_t capacity;
} PairStack;

void pair_stack_init(PairStack* stack);

Pair pair_stack_pop(PairStack* stack);

void pair_stack_push(PairStack* stack, Pair pair);

bool pair_stack_empty(PairStack* stack);

void pair_stack_destroy(PairStack* stack);

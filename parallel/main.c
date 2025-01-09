#include <stddef.h>

#include "common/io.h"
#include "common/sumset.h"
#include "parallel/smart_sumset.h"
#include "parallel/safe_stack.h"
#include "parallel/pool.h"

typedef struct{
    InputData* input_data;
    SafeStack* stack;
    Solution* best_solution;
} WorkerData;

void* worker(void* arg){
    WorkerData data = *(WorkerData*)arg;
    
    Pool pool;
    pool_init(&pool);
    
    while(true){
        SafeStackPair pair = safe_stack_pop(data.stack);
        if(pair.a == NULL){
            break; // end of the program
        }
    }
}

bool is_stack_small(SafeStack* stack, WorkerData* data){
    return atomic_load(&stack->size) < data->input_data->t * 2;
}

void continue_recursively(Sumset* a, Sumset* b, SafeStack* stack, Pool* pool, WorkerData* data){
    for (size_t i = a->last; i <= data->input_data->d; ++i) {
        if (!does_sumset_contain(b, i)) {
            SmartSumset* a_with_i = pool_new_empty(pool);
            sumset_add(&a_with_i, a, i);
            recursive_solve(&a_with_i, b, stack, pool, data);
        }
    }
}

void recursive_solve(SmartSumset* smart_a, SmartSumset* smart_b, SafeStack* stack, Pool* pool, WorkerData* data){
    if(a->sum > b->sum){
        return recursive_solve(b, a, stack, pool, data);
    }

    if(is_sumset_intersection_trivial(a, b)){
        if(is_stack_small(stack, data)){
            
        } else{

        }
        for(size_t i = a->last; i <= data->input_data->d; i++){
            if(!does_sumset_contain(b, i)){
                Sumset a_with_i;
                sumset_add(&a_with_i, a, i);
                recursive_solve(&a_with_i, b, stack);
            }
        }
    } else if(a->sum == b->sum && get_sumset_intersection_size(a, b) == 2){
        SafeStackPair pair = safe_stack_pair_construct(a, b);
        safe_stack_push(stack, pair);
    }
}

int main()
{
    InputData input_data;
    input_data_read(&input_data);
    // input_data_init(&input_data, 8, 10, (int[]){0}, (int[]){1, 0});

    Solution best_solution;
    solution_init(&best_solution);

    // ...

    solution_print(&best_solution);
    return 0;
}

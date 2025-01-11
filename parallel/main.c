#include <pthread.h>
#include <stddef.h>

#include "common/io.h"
#include "common/sumset.h"
#include "parallel/parallel_worker.h"
#include "parallel/pool.h"
#include "parallel/safe_stack.h"
#include "parallel/smart_sumset.h"

SafeStackPair get_initial_pair(InputData* input_data, Pool* pool)
{
    SmartSumset* smart_a = pool_new_from_existing(pool, input_data->a_start);
    smart_sumset_set_parent(smart_a, NULL);
    SmartSumset* smart_b = pool_new_from_existing(pool, input_data->b_start);
    smart_sumset_set_parent(smart_b, NULL);
    return safe_stack_pair_construct(smart_a, smart_b);
}

void run_threads(InputData* input_data, SafeStack* stack, pthread_t* threads, WorkerData* worker_data)
{
    // prepare the stack
    safe_stack_init(stack, input_data);
    Pool pool; // by far not the most efficient way to get the two initial sumsets, but it works
    pool_init(&pool);
    SafeStackPair initial_pair = get_initial_pair(input_data, &pool);
    safe_stack_push(stack, initial_pair);

    // start the threads
    for (size_t i = 0; i < input_data->t; i++) {
        worker_data[i].input_data = input_data;
        worker_data[i].stack = stack;
        // each thread has their own solution which needs to be merged at the end
        solution_init(&worker_data[i].best_solution);
        pthread_create(&threads[i], NULL, worker_thread, &worker_data[i]);
    }

    // and wait for them to finish
    for (size_t i = 0; i < input_data->t; i++) {
        pthread_join(threads[i], NULL);
    }

    pool_close(&pool);
    safe_stack_destroy(stack);
}

static inline void solution_merge(Solution* best_solution, Solution* worker_solution)
{
    if (worker_solution->sum > best_solution->sum) {
        *best_solution = *worker_solution;
    }
}

void collect_threads_results(WorkerData* worker_data, Solution* best_solution, size_t t)
{
    for (size_t i = 0; i < t; i++) {
        solution_merge(best_solution, &worker_data[i].best_solution);
    }
}

int main()
{
    InputData input_data;
    input_data_read(&input_data);
    // input_data_init(&input_data, 8, 10, (int[]){0}, (int[]){1, 0});

    Solution best_solution;
    solution_init(&best_solution);

    pthread_t threads[input_data.t];
    WorkerData worker_data[input_data.t];
    SafeStack stack;

    run_threads(&input_data, &stack, threads, worker_data);
    collect_threads_results(worker_data, &best_solution, input_data.t);

    solution_print(&best_solution);
    return 0;
}

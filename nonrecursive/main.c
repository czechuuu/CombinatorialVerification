#include <stddef.h>
#include <stdio.h>

#include "common/io.h"
#include "common/sumset.h"

#include "nonrecursive/shared_sumset.h"
#include "nonrecursive/sumset_stack.h"
#include "nonrecursive/sumset_pool.h"

static InputData input_data;
static Solution best_solution;

static void solve(Sumset const initial_a, Sumset const initial_b)
{
    PairStack stack;
    pair_stack_init(&stack);
    pair_stack_push(&stack, pair_construct(pool_new_from_existing(initial_a), pool_new_from_existing(initial_b)));

    while (!pair_stack_empty(&stack)) {
        Pair pair = pair_stack_pop(&stack);
        SharedSumset* sh_a = pair.a;
        SharedSumset* sh_b = pair.b;

        // swap if wrong order
        if (shared_sumset_get(sh_a).sum > shared_sumset_get(sh_b).sum) {
            SharedSumset* tmp = sh_a;
            sh_a = sh_b;
            sh_b = tmp;
        }

        Sumset* a = shared_sumset_get_ptr(sh_a);
        Sumset* b = shared_sumset_get_ptr(sh_b);

        if (is_sumset_intersection_trivial(a, b)) {
            for (size_t i = a->last; i <= input_data.d; ++i) {
                if (!does_sumset_contain(b, i)) {
                    SharedSumset* new_a_sh = pool_new_empty();
                    sumset_add(shared_sumset_get_ptr(new_a_sh), a, i);
                    shared_sumset_set_parent(new_a_sh, sh_a);
                    pair_stack_push(&stack, pair_construct(new_a_sh, sh_b));
                    shared_sumset_inc_ref(sh_b); // new copy on the stack
                }
            }
        } else if (a->sum == b->sum && get_sumset_intersection_size(a, b) == 2) {
            if (b->sum > best_solution.sum) {
                solution_build(&best_solution, &input_data, a, b);
            }
        }

        // potentially free the memory if it isn't anywhere on the stack anymore
        pool_release(sh_a);
        pool_release(sh_b);
    }

    pair_stack_destroy(&stack);
    pool_close();
}

int main()
{
    input_data_read(&input_data);
    // input_data_init(&input_data, 8, 10, (int[]){0}, (int[]){1, 0});

    solution_init(&best_solution);

    solve(input_data.a_start, input_data.b_start);

    solution_print(&best_solution);
    return 0;
}

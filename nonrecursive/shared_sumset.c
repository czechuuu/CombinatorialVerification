#include "nonrecursive/shared_sumset.h"

#include <stdlib.h>

SharedSumset* shared_sumset_init(Sumset const sumset)
{
    SharedSumset* shared_sumset = malloc(sizeof(SharedSumset));
    if(!shared_sumset){
        exit(1);
    }

    shared_sumset->ptr = malloc(sizeof(Sumset));
    if(!shared_sumset->ptr){
        free(shared_sumset);
        exit(1);
    }

    *shared_sumset->ptr = sumset;
    shared_sumset->ref_count = 1;
    shared_sumset->child = NULL;
    return shared_sumset;
}

SharedSumset* shared_sumset_init_child(Sumset const sumset, SharedSumset* child)
{
    SharedSumset* shared_sumset = shared_sumset_init(sumset);
    shared_sumset->child = child;
    if(child != NULL){
        shared_sumset_inc_ref(child);
    }
    return shared_sumset;
}

void shared_sumset_inc_ref(SharedSumset* shared_sumset)
{
    shared_sumset->ref_count++;
}

void shared_sumset_dec_ref(SharedSumset* shared_sumset)
{
    shared_sumset->ref_count--;
    if (shared_sumset->ref_count == 0)
    {
        free(shared_sumset->ptr);
        free(shared_sumset);
        if(shared_sumset->child){
            shared_sumset_dec_ref(shared_sumset->child);
        }
    }
}

Sumset* shared_sumset_get(SharedSumset* shared_sumset)
{
    return shared_sumset->ptr;
}
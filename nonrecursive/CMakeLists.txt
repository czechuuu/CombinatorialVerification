add_executable(nonrecursive main.c shared_sumset.c sumset_stack.c) # suboptimal linking
target_link_libraries(nonrecursive io err atomic)

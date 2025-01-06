add_executable(nonrecursive main.c) # suboptimal linking
target_link_libraries(nonrecursive io err atomic)

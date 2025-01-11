cmake_minimum_required(VERSION 3.12)

project(multiset LANGUAGES C)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-uninitialized -Wno-unused-function -Werror)
add_compile_options(-pthread)
add_link_options(-pthread)

# uncommentted this
# add_compile_options(-fsanitize=thread)
# add_link_options(-fsanitize=thread)

# TODO remove for profling
# add_compile_options(-pg)
# add_link_options(-pg)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-march=native -O3)
endif()

# Find clang-tidy - i added
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    set(CMAKE_C_CLANG_TIDY "${CLANG_TIDY_EXE}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
endif()

# Optionally: include-what-you-use to minimize #include-s.
# find_program(IWYU_PATH NAMES include-what-you-use iwyu)
# if(IWYU_PATH)
#     set(CMAKE_C_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})
# else()
#     message(WARNING "Could not find the program include-what-you-use / iwyu")
# endif()


# add_compile_options(-DLOG_SUMSET=1)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(common)
add_subdirectory(reference)
add_subdirectory(nonrecursive)
add_subdirectory(parallel)
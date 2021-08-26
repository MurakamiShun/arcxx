cmake_minimum_required(VERSION 3.11)

project(tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.7
)

FetchContent_MakeAvailable(Catch2)

if(ENABLE_PROFILE)
set(compile_options -Wall -Wextra -pedantic -Werror -pg -g)
set(link_options -pg)
else()
set(compile_options -Wall -Wextra -pedantic -Werror)
set(link_options)
endif()

# Benchmark
add_executable(active_record_bench
    inserting_benchmark.cpp
)
target_link_libraries(active_record_bench PRIVATE Catch2::Catch2)
target_compile_options(active_record_bench PUBLIC ${compile_options})
target_link_options(active_record_bench PUBLIC ${link_options})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)

catch_discover_tests(active_record_bench)
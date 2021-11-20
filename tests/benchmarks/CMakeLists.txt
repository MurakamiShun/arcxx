cmake_minimum_required(VERSION 3.11)

project(tests LANGUAGES CXX)

if(MSVC)
set(CMAKE_CXX_STANDARD 23) # c++latest
else()
set(CMAKE_CXX_STANDARD 20)
endif()

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.7
)

FetchContent_MakeAvailable(Catch2)

if(MSVC)
set(compile_options)
else()
set(compile_options -Wall -Wextra -pedantic -Werror)
endif()

# Benchmark
add_executable(active_record_bench
    main.cpp
    inserting_benchmark.cpp
    long_SQL_statement_benchmark.cpp
)
target_link_libraries(active_record_bench PRIVATE Catch2::Catch2)
target_compile_options(active_record_bench PUBLIC ${compile_options})
target_link_options(active_record_bench PUBLIC ${link_options})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)

if(MSVC)
    add_test(NAME active_record_bench COMMAND active_record_bench.exe)
else()
    add_test(NAME active_record_bench COMMAND active_record_bench)
endif()
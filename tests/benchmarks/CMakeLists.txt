cmake_minimum_required(VERSION 3.11)

project(tests LANGUAGES CXX)

if(MSVC)
    find_package(Catch2 CONFIG REQUIRED)
    set(compile_options /W3)
    set(compile_feature cxx_std_23) # c++latest
else()
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1
    )

    FetchContent_MakeAvailable(Catch2)
    set(compile_options -Wall -Wextra -pedantic -Werror)
    set(compile_feature cxx_std_20)
endif()

# Benchmark
add_executable(arcxx_bench
    inserting_benchmark.cpp
    long_SQL_statement_benchmark.cpp
)
target_link_libraries(arcxx_bench PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_compile_options(arcxx_bench PRIVATE ${compile_options})
target_include_directories(arcxx_bench PRIVATE ../../include)
target_compile_features(arcxx_bench PRIVATE ${compile_feature})

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)

if(MSVC)
    add_test(NAME arcxx_bench COMMAND arcxx_bench.exe)
else()
    add_test(NAME arcxx_bench COMMAND arcxx_bench)
endif()
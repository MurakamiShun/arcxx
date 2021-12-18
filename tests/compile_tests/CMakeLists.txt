cmake_minimum_required(VERSION 3.11)

project(tests LANGUAGES CXX)

if(MSVC)
    set(compile_options /W3)
    set(compile_feature cxx_std_23) # c++latest
else()
    set(compile_options -Wall -Wextra -pedantic -Werror)
    set(compile_feature cxx_std_20)
endif()

add_executable(model_query model_query.cpp)
target_compile_options(model_query PUBLIC ${compile_options})
target_include_directories(model_query PUBLIC ../../include)
target_compile_features(model_query PRIVATE ${compile_feature})

add_executable(query_vector query_vector.cpp)
target_compile_options(query_vector PUBLIC ${compile_options})
target_include_directories(query_vector PUBLIC ../../include)
target_compile_features(query_vector PRIVATE ${compile_feature})

add_executable(query_scalar query_scalar.cpp)
target_compile_options(query_scalar PUBLIC ${compile_options})
target_include_directories(query_scalar PUBLIC ../../include)
target_compile_features(query_scalar PRIVATE ${compile_feature})

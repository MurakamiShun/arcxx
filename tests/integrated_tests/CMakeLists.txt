cmake_minimum_required(VERSION 3.16)

project(tests LANGUAGES CXX)

if(MSVC)
    find_package(Catch2 CONFIG REQUIRED)
    
    set(compile_feature cxx_std_23) # c++latest
    if(TARGET_SQL MATCHES "PostgreSQL")
        find_package(PostgreSQL REQUIRED)
        set(compile_options /W3 /DPOSTGRESQL_TEST)
        set(link_library Catch2::Catch2 Catch2::Catch2WithMain PostgreSQL::PostgreSQL)
    else()
        find_package(unofficial-sqlite3 CONFIG REQUIRED)
        set(compile_options /W3 /DSQLITE_TEST)
        set(link_library Catch2::Catch2 Catch2::Catch2WithMain unofficial::sqlite3::sqlite3)
    endif()
else()
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1
    )
    FetchContent_MakeAvailable(Catch2)
    set(compile_feature cxx_std_20)
    if(TARGET_SQL MATCHES "PostgreSQL")
        find_package(PostgreSQL REQUIRED)
        set(compile_options -Wall -Wextra -pedantic -Werror -DPOSTGRESQL_TEST)
        set(link_library Catch2::Catch2 Catch2::Catch2WithMain PostgreSQL::PostgreSQL)
    else()
        set(compile_options -Wall -Wextra -pedantic -Werror -DSQLITE_TEST)
        set(link_library Catch2::Catch2 Catch2::Catch2WithMain sqlite3)
    endif()
endif()


# Integrated Tests
add_executable(active_record_IT
    table_creation_test.cpp
    select_test.cpp
    insert_test.cpp
    delete_test.cpp
    update_test.cpp
    join_test.cpp
    aggregation_test.cpp
    group_test.cpp
    find_test.cpp
)
target_link_libraries(active_record_IT PRIVATE ${link_library})
target_compile_options(active_record_IT PUBLIC ${compile_options})
target_include_directories(active_record_IT PUBLIC ../../include)
target_compile_features(active_record_IT PRIVATE ${compile_feature})
target_precompile_headers(active_record_IT PUBLIC <active_record.hpp> <filesystem>)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)

if(MSVC)
    add_test(NAME active_record_IT COMMAND active_record_IT.exe)
else()
    add_test(NAME active_record_IT COMMAND active_record_IT)
endif()
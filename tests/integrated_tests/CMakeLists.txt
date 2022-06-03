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
add_executable(arcxx_IT
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
target_link_libraries(arcxx_IT PRIVATE ${link_library})
target_compile_options(arcxx_IT PRIVATE ${compile_options})
target_include_directories(arcxx_IT PRIVATE ../../include)
target_compile_features(arcxx_IT PRIVATE ${compile_feature})
target_precompile_headers(arcxx_IT PRIVATE <arcxx.hpp> <filesystem>)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)

if(MSVC)
    add_test(NAME arcxx_IT COMMAND arcxx_IT.exe)
else()
    add_test(NAME arcxx_IT COMMAND arcxx_IT)
endif()
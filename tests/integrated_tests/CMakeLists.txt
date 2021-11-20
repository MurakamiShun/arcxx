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

if(TARGET_SQL MATCHES "PostgreSQL")
    if(MSVC)
        set(compile_options /DPOSTGRESQL_TEST)
    else()
        set(compile_options -Wall -Wextra -pedantic -Werror -DPOSTGRESQL_TEST)
    endif()
    set(link_library Catch2::Catch2 pq)
else()
    if(MSVC)
        set(compile_options /DSQLITE_TEST)
    else()
        set(compile_options -Wall -Wextra -pedantic -Werror -DSQLITE_TEST)
    endif()
    set(link_library Catch2::Catch2 sqlite3)
endif()

# Integrated Tests
add_executable(active_record_IT
    main.cpp
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

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)

if(MSVC)
    add_test(NAME active_record_IT COMMAND active_record_IT.exe)
else()
    add_test(NAME active_record_IT COMMAND active_record_IT)
endif()
cmake_minimum_required(VERSION 3.11)

project(tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -pedantic -Werror)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.7
)

FetchContent_MakeAvailable(Catch2)


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
target_link_libraries(active_record_IT PRIVATE Catch2::Catch2 sqlite3)

# Unit Tests
add_executable(active_record_UT
    main.cpp
    model_test.cpp
)
target_link_libraries(active_record_UT PRIVATE Catch2::Catch2 sqlite3)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)

catch_discover_tests(active_record_IT)
catch_discover_tests(active_record_UT)
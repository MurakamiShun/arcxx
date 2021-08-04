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


add_executable(delete_test delete_test.cpp)
target_link_libraries(delete_test PRIVATE Catch2::Catch2 sqlite3)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(CTest)
include(Catch)

catch_discover_tests(delete_test)
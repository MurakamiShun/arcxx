cmake_minimum_required(VERSION 3.14)

project(find_cloumn_example LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    ARCXX
    GIT_REPOSITORY https://github.com/akisute514/arcxx
    GIT_TAG main
)
FetchContent_MakeAvailable(ARCXX)

find_package(SQLite3 REQUIRED)

add_executable(find_cloumn main.cpp)
target_link_libraries(find_cloumn PRIVATE ARCXX::ARCXX SQLite::SQLite3)
target_compile_features(find_cloumn PRIVATE cxx_std_20)
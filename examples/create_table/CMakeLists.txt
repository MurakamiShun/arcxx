cmake_minimum_required(VERSION 3.14)

project(create_table_example LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    ARCXX
    GIT_REPOSITORY https://github.com/akisute514/arcxx
    GIT_TAG main
)
FetchContent_MakeAvailable(ARCXX)

find_package(SQLite3 REQUIRED)

add_executable(create_table main.cpp)
target_link_libraries(create_table PRIVATE ARCXX::ARCXX SQLite::SQLite3)
target_compile_features(create_table PRIVATE cxx_std_20)
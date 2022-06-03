cmake_minimum_required(VERSION 3.14)

project(insert_data_example LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    ARCXX
    GIT_REPOSITORY https://github.com/akisute514/arcxx
    GIT_TAG main
)
FetchContent_MakeAvailable(ARCXX)

find_package(SQLite3 REQUIRED)

add_executable(insert_data main.cpp)
target_link_libraries(insert_data PRIVATE ARCXX::ARCXX SQLite::SQLite3)
target_compile_features(insert_data PRIVATE cxx_std_20)
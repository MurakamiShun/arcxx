cmake_minimum_required(VERSION 3.11)

project(create_table_example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(ActiveRecord)

add_executable(create_table main.cpp)
target_link_libraries(create_table PRIVATE ActiveRecord sqlite3)
cmake_minimum_required(VERSION 3.14)

project(find_cloumn_example LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    ActiveRecord
    GIT_REPOSITORY https://github.com/akisute514/active_record_cpp
    GIT_TAG main
)
FetchContent_MakeAvailable(ActiveRecord)

find_package(SQLite3 REQUIRED)

add_executable(find_cloumn main.cpp)
target_link_libraries(find_cloumn PRIVATE ActiveRecord::ActiveRecord SQLite::SQLite3)
target_compile_features(find_cloumn PUBLIC cxx_std_20)
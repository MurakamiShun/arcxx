cmake_minimum_required(VERSION 3.9)
enable_testing()

project(active_record_test)
set(CMAKE_CXX_STANDARD 20)

# insert test
add_executable(insert_test insert.cpp)
target_link_libraries(insert_test sqlite3)
add_test(
    NAME insert_test
    COMMAND $<TARGET_FILE:insert_test>
)

# delete test
add_executable(delete_test delete.cpp)
target_link_libraries(delete_test sqlite3)
add_test(
    NAME delete_test
    COMMAND $<TARGET_FILE:delete_test>
)

# update test
add_executable(update_test update.cpp)
target_link_libraries(update_test sqlite3)
add_test(
    NAME update_test
    COMMAND $<TARGET_FILE:update_test>
)
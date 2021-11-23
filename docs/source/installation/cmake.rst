#######################
Installation with CMake
#######################

Quick Start
===========

Make your project folder and source files.
And make CMakeLists.txt under your project folder.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.14)
    project(your_prject_name LANGUAGES CXX)
    add_executable(${PROJECT_NAME} main.cpp)
    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

    include(FetchContent)
    FetchContent_Declare(
        ActiveRecord
        GIT_REPOSITORY https://github.com/akisute514/active_record_cpp
        GIT_TAG main
    )
    FetchContent_MakeAvailable(ActiveRecord)
    target_link_libraries(${PROJECT_NAME} PRIVATE ActiveRecord::ActiveRecord)


Type below commands to build your project.

.. code-block:: bash

    $ cmake -S . -B build
    $ cmake --build build


Installation
============

You can use CMake to install Active Record C++ package.

.. code-block:: bash

    $ git clone https://github.com/akisute514/active_record_cpp.git
    $ cd active_record_cpp
    $ cmake -S . -B build
    $ cmake --install build
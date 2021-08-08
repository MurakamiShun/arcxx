#######################
Installation with CMake
#######################


Installation
============

We can use CMake to install active_record_cpp package.

.. code-block:: bash

    $ git clone https://github.com/akisute514/active_record_cpp.git
    $ cd active_record_cpp
    $ cmake -S . -B build
    $ cmake --build build
    $ cmake --install build


Quick Start
===========

Make your project folder and source files.
And make CMakeLists.txt under your project folder.

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.9)

    project(your_prject_name LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 20)
    find_package(active_record_cpp)
    add_executable(${PROJECT_NAME} main.cpp)
    target_link_libraries(${PROJECT_NAME} PRIVATE active_record_cpp)

Type below commands to build your project.

.. code-block:: bash

    $ cmake -S . -B build
    $ cmake --build build

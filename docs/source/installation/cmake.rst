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
        ARCXX
        GIT_REPOSITORY https://github.com/akisute514/arcxx
        GIT_TAG main
    )
    FetchContent_MakeAvailable(ARCXX::ARCXX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ARCXX::ARCXX)


Type below commands to build your project.

.. code-block:: bash

    $ cmake -S . -B build
    $ cmake --build build


Installation
============

You can use CMake to install ARCXX package.

.. code-block:: bash

    $ git clone https://github.com/akisute514/arcxx.git
    $ cd arcxx
    $ cmake -S . -B build
    $ cmake --install build
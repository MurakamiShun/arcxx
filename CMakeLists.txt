cmake_minimum_required(VERSION 3.9)

if(NOT DEFINED PROJECT_NAME)
    set(ARCXX_NOT_SUBPROJECT ON)
else()
    set(ARCXX_NOT_SUBPROJECT OFF)
endif()

project(ARCXX
    VERSION 0.1
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/MurakamiShun/arcxx"
)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(ARCXX_NOT_SUBPROJECT)
    install(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/arcxx.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/arcxx
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
    )

    install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endif()
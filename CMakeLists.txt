cmake_minimum_required(VERSION 3.25)
project(graphAlg LANGUAGES C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(C_STANDARD 17)
set(CMAKE_C_FLAGS "-Wall -Werror")

set(PROJECT_AUTHOR "David C. Days")
set(PROJECT_VENDOR "Silicon Holler")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

find_package(PkgConfig REQUIRED)
# Fedora often ships .pc under /usr/lib/pkgconfig while pkg-config defaults to lib64.
if(DEFINED ENV{PKG_CONFIG_PATH} AND NOT "$ENV{PKG_CONFIG_PATH}" STREQUAL "")
    set(_pkg_path "$ENV{PKG_CONFIG_PATH}")
else()
    set(_pkg_path "/usr/lib/pkgconfig:/usr/share/pkgconfig")
endif()
set(ENV{PKG_CONFIG_PATH} "${_pkg_path}")
pkg_check_modules(GRAPHDATA REQUIRED graphdata)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)
add_subdirectory(examples)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/packaging.cmake)

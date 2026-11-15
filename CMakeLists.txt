cmake_minimum_required(VERSION 3.25)
project(graphAlg LANGUAGES C)

include(GNUInstallDirs)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Append — do not clobber distro/rpmbuild CFLAGS
set(CMAKE_C_STANDARD 17)
add_compile_options(-Wall -Werror)

set(PROJECT_AUTHOR "David C. Days")
set(PROJECT_VENDOR "Silicon Holler")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION
    ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

find_package(PkgConfig REQUIRED)
# Prefer lib64 (COPR/Fedora), keep /usr/lib as fallback for old tarball installs.
if(NOT DEFINED ENV{PKG_CONFIG_PATH} OR "$ENV{PKG_CONFIG_PATH}" STREQUAL "")
    set(ENV{PKG_CONFIG_PATH}
        "/usr/lib64/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig")
endif()
pkg_check_modules(GRAPHDATA REQUIRED graphdata)

add_subdirectory(src)

option(BUILD_TESTING "Build unit tests (requires Check)" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
add_subdirectory(examples)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/packaging.cmake)

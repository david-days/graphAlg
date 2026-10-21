find_package(Check REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/tests/bin)

add_subdirectory(smoke)
add_subdirectory(util)

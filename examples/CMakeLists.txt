add_executable(version_demo
        version_demo.c
)

target_include_directories(version_demo PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(version_demo PRIVATE
        ${PROJECT_NAME}
)

add_test(NAME version_demo COMMAND version_demo)

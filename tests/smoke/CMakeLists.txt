add_executable(smoketests
        smoketests.c
)

target_include_directories(smoketests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(smoketests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(smoketests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME smoketests COMMAND smoketests)

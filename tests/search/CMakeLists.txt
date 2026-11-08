# graphAlg search (BFS path / BFS-from) tests

add_executable(bfspathtests
        bfspathtests.c
)

target_include_directories(bfspathtests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(bfspathtests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(bfspathtests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME bfspathtests COMMAND bfspathtests)

add_executable(bfsfromtests
        bfsfromtests.c
)

target_include_directories(bfsfromtests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(bfsfromtests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(bfsfromtests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME bfsfromtests COMMAND bfsfromtests)

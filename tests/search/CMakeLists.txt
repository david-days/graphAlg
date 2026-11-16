# graphAlg search (BFS / DFS / connected components / Dijkstra path) tests

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

add_executable(dfsfromtests
        dfsfromtests.c
)

target_include_directories(dfsfromtests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(dfsfromtests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(dfsfromtests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME dfsfromtests COMMAND dfsfromtests)

add_executable(connectedcomponentstests
        connectedcomponentstests.c
)

target_include_directories(connectedcomponentstests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(connectedcomponentstests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(connectedcomponentstests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME connectedcomponentstests COMMAND connectedcomponentstests)

add_executable(dijkstrapathtests
        dijkstrapathtests.c
)

target_include_directories(dijkstrapathtests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(dijkstrapathtests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
        m
)

target_compile_options(dijkstrapathtests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME dijkstrapathtests COMMAND dijkstrapathtests)

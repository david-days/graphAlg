# Queue structure tests

add_executable(queuetests
        queuetests.c
)

target_include_directories(queuetests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(queuetests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(queuetests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME queuetests COMMAND queuetests)

# Stack and heap structure tests
add_executable(stacktests
        stacktests.c
)

target_include_directories(stacktests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(stacktests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(stacktests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME stacktests COMMAND stacktests)

add_executable(heaptests
        heaptests.c
)

target_include_directories(heaptests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(heaptests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(heaptests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME heaptests COMMAND heaptests)

# List structure tests

add_executable(listtests
        listtests.c
)

target_include_directories(listtests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(listtests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(listtests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME listtests COMMAND listtests)


# Sorting algorithms tests
add_executable(quicksorttests
        quicksorttests.c
)

target_include_directories(quicksorttests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(quicksorttests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(quicksorttests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME quicksorttests COMMAND quicksorttests)

add_executable(selectionsorttests
        selectionsorttests.c
)

target_include_directories(selectionsorttests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(selectionsorttests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(selectionsorttests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME selectionsorttests COMMAND selectionsorttests)

add_executable(mergesorttests
        mergesorttests.c
)

target_include_directories(mergesorttests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(mergesorttests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(mergesorttests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME mergesorttests COMMAND mergesorttests)

# Hash utility tests
add_executable(hashtests
        hashtests.c
)

target_include_directories(hashtests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(hashtests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(hashtests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME hashtests COMMAND hashtests)

# Hash set tests
add_executable(hashsettests
        hashsettests.c
)

target_include_directories(hashsettests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(hashsettests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(hashsettests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME hashsettests COMMAND hashsettests)

# Hash table tests
add_executable(hashtabletests
        hashtabletests.c
)

target_include_directories(hashtabletests PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${GRAPHDATA_INCLUDE_DIRS}
        ${CHECK_INCLUDE_DIRS}
)

target_link_libraries(hashtabletests PRIVATE
        ${PROJECT_NAME}
        ${GRAPHDATA_LIBRARIES}
        ${CHECK_LIBRARIES}
)

target_compile_options(hashtabletests PRIVATE ${GRAPHDATA_CFLAGS_OTHER})

add_test(NAME hashtabletests COMMAND hashtabletests)


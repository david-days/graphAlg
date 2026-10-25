add_library(${PROJECT_NAME} SHARED
        graphAlg.c
        graphAlgCut.c
        util/queue.c
        util/stack.c
        util/heap.c
        util/lists.c
        util/quicksort.c
        util/selectionsort.c
        util/mergesort.c
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        ${GRAPHDATA_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${GRAPHDATA_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE ${GRAPHDATA_CFLAGS_OTHER})
target_compile_definitions(${PROJECT_NAME} PRIVATE
        GRAPHALG_VERSION_STRING="${PROJECT_VERSION}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

include(${PROJECT_SOURCE_DIR}/cmake/packaging.cmake)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
)

install(FILES
        ${PROJECT_SOURCE_DIR}/include/graphAlg.h
        ${PROJECT_SOURCE_DIR}/include/graphAlgCut.h
        DESTINATION include
)

configPackaging()

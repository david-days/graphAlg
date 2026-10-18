macro(configPackaging)

    set(GRAPHALG_PKG_VENDOR "Silicon Holler")
    set(GRAPHALG_PKG_NAME "graphalg")
    set(GRAPHALG_PKG_MAINTAINER "David C Days <david.c.days@gmail.com>")
    set(GRAPHALG_PKG_SUMMARY "Graph algorithms on the graphData public API")
    set(GRAPHALG_PKG_DESCR
        "Native library of graph algorithms that depend only on installed graphData")
    set(GRAPHALG_PKG_VERSION ${PROJECT_VERSION})
    set(GRAPHALG_PROJECT_HOME "https://github.com/david-days/graphAlg")

    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
    set(CPACK_PACKAGE_DIRECTORY
        "${PROJECT_SOURCE_DIR}/output/${CMAKE_OS_NAME}/${CMAKE_OS_VERSION}/${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_PACKAGE_VERSION ${GRAPHALG_PKG_VERSION})
    set(CPACK_PACKAGE_NAME ${GRAPHALG_PKG_NAME})
    set(CPACK_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CMAKE_OS_NAME}.${CMAKE_OS_VERSION}.${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME
        "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")
    set(CPACK_VERBATIM_VARIABLES TRUE)
    set(CPACK_SOURCE_IGNORE_FILES
            /\\\\..*
            /.*build.*/
            ${PROJECT_BINARY_DIR}
    )

    if(CMAKE_OS_NAME MATCHES fedora OR CMAKE_OS_NAME MATCHES redhat OR CMAKE_OS_NAME MATCHES rhel)
        message(STATUS "Building on Fedora/Redhat")
        set(CPACK_GENERATOR "RPM;TGZ")
        set(CPACK_RPM_PACKAGE_SUMMARY ${GRAPHALG_PKG_SUMMARY})
        set(CPACK_RPM_PACKAGE_DESCRIPTION ${GRAPHALG_PKG_DESCR})
        set(CPACK_RPM_PACKAGE_VENDOR ${GRAPHALG_PKG_VENDOR})
        set(CPACK_RPM_PACKAGE_VERSION ${GRAPHALG_PKG_VERSION})
        set(CPACK_RPM_PACKAGE_RELEASE 1)
        set(CPACK_RPM_PACKAGE_GROUP "Applications/Engineering")
        set(CPACK_RPM_PACKAGE_URL ${GRAPHALG_PROJECT_HOME})
        set(CPACK_RPM_PACKAGE_REQUIRES "graphdata")
    elseif(CMAKE_OS_NAME MATCHES Debian OR CMAKE_OS_NAME MATCHES Ubuntu)
        message(STATUS "Building on Debian/Ubuntu (deferred packaging)")
        set(CPACK_GENERATOR "TGZ")
    else()
        set(CPACK_GENERATOR "TGZ")
    endif()

    configure_file(
            ${PROJECT_SOURCE_DIR}/conf.in/graphalg.pc.in
            ${CMAKE_CURRENT_BINARY_DIR}/graphalg.pc
            @ONLY
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/graphalg.pc
            DESTINATION lib/pkgconfig
    )

    include(CPack)
endmacro()

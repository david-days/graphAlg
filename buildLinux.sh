#!/bin/bash

set -euo pipefail

PROJECT_DIR=$(pwd)

LINUX_BUILD_DIR=${PROJECT_DIR}/build-linux-x64
BINARY_OUT_DIR=${PROJECT_DIR}/output
LINUX_TOOLCHAIN=${PROJECT_DIR}/cmake/linux-x64.cmake

# graphdata.pc from the v1 tarball/RPM often lands in /usr/lib/pkgconfig.
export PKG_CONFIG_PATH="/usr/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

RUN_TESTS=""
OUTPUT_VARS="false"
OUTPUT_ALL_VARS=0
PACKAGE="false"

while getopts ":cithvxp" option; do
    case $option in
        c)
            echo cleaning...
            rm -rvf "${LINUX_BUILD_DIR}"/*
            rm -rvf "${BINARY_OUT_DIR}"/*
            exit 0
            ;;
        i)
            cd "${LINUX_BUILD_DIR}"
            make install
            exit 0
            ;;
        t)
            RUN_TESTS="TEST"
            ;;
        p)
            PACKAGE="true"
            ;;
        v)
            OUTPUT_VARS="true"
            ;;
        x)
            OUTPUT_ALL_VARS=1
            ;;
        h)
            echo "Usage: ./buildLinux.sh [-c | -i | -t | -v | -x | -p | -h]"
            echo "  No args  (Re)Build"
            echo "  -c Clean.  Clean output folders and exit"
            echo "  -i Install.  Install from the last build"
            echo "  -t Test.  Build and run tests, then exit"
            echo "  -v Verbose.  Print CMAKE build variables and exit"
            echo "  -x Extra verbose.  Print all CMAKE build variables"
            echo "  -p Package.  Generate CPack packages"
            echo "  -h Help.  Print this help and exit"
            exit 0
            ;;
        *)
            ;;
    esac
done

if [ ! -d "${LINUX_BUILD_DIR}" ]; then
    mkdir "${LINUX_BUILD_DIR}"
fi

if [ "${OUTPUT_VARS}" == "true" ]; then
    cmake . -LH --toolchain "${LINUX_TOOLCHAIN}" -B "${LINUX_BUILD_DIR}"
    exit 0
fi

cmake . --toolchain "${LINUX_TOOLCHAIN}" -B "${LINUX_BUILD_DIR}" \
    -DPRINT_ALL_VARS="${OUTPUT_ALL_VARS}" \
    && cd "${LINUX_BUILD_DIR}" && make all

if [ "${RUN_TESTS}" == "TEST" ]; then
    make test
fi

if [ "${PACKAGE}" == "true" ]; then
    make package
fi

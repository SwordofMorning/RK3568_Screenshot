PROJECT(RK3568_Screenshot)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

SET(COMPILER_PATH "/home/xjt/Gogs/OK3568-linux-source/buildroot/output/OK3568/host/bin/")

SET(CMAKE_C_COMPILER ${COMPILER_PATH}aarch64-buildroot-linux-gnu-gcc)

ADD_SUBDIRECTORY(src bin)
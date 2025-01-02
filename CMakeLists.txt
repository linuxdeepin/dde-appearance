# SPDX-FileCopyrightText: None
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.16)

project(dde-appearance)

cmake_policy(SET CMP0160 OLD)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()
include(GNUInstallDirs)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror=return-type -fno-omit-frame-pointer -Wextra")
endif()

set(QT_VERSION_MAJOR 6)
set(DTK_VERSION_MAJOR 6)

set(TRANS_FILE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}src/service/impl/appearancemanager.cpp)
set(PLUGIN_NAME "plugin-dde-appearance")

add_subdirectory(src)
add_subdirectory(misc)
add_subdirectory(fakewm)

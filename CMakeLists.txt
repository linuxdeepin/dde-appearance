# SPDX-FileCopyrightText: None
#
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required(VERSION 3.11)

project(dde-appearance)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()
include(GNUInstallDirs)

add_subdirectory(src)
add_subdirectory(misc)

# SPDX-FileCopyrightText: None
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include(FindPkgConfig)
find_package(PkgConfig REQUIRED)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus Concurrent Gui)
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)
find_package(KF6Config REQUIRED)
find_package(KF6WindowSystem REQUIRED)
find_package(KF6GlobalAccel REQUIRED)

set(SRCS
    ./dbus/deepinwmfaker.h
    ./dbus/deepinwmfaker.cpp
    main.cpp
)

qt_add_dbus_adaptor(
    wm_SRCS
    ../dbus/com.deepin.wm.xml
    ./dbus/deepinwmfaker.h
    DeepinWMFaker
)

add_executable(dde-fakewm
    ${SRCS}
    ${wm_SRCS}
)

target_link_libraries(dde-fakewm
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Gui
    Dtk${DTK_VERSION_MAJOR}::Core
    KF6::ConfigCore
    KF6::WindowSystem
    KF6::GlobalAccel
)

install(TARGETS dde-fakewm DESTINATION ${CMAKE_INSTALL_BINDIR})

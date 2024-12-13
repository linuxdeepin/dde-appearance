# SPDX-FileCopyrightText: None
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-DQT_NO_KEYWORDS)

find_package(PkgConfig REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus Concurrent Gui)
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED Core Gui)
#find_package(KF6Config REQUIRED)
#find_package(KF6WindowSystem REQUIRED)
#find_package(KF6GlobalAccel REQUIRED)

pkg_check_modules(Gio-2.0 REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET xcursor xfixes x11)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0 gdk-3.0)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb-render xcb xcb-randr xcb-cursor)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(OPENSSL REQUIRED IMPORTED_TARGET openssl)

add_definitions(-DUSE_QT)

qt_add_dbus_adaptor(ADAPTER_SOURCES ../../dbus/org.deepin.dde.Appearance1.xml ./dbus/appearance1.h Appearance1)

file(GLOB SRCS ./plugin.cpp ./dbus/*.h ./dbus/*.cpp ./impl/*.h ./impl/*.cpp ./modules/*.h ./modules/*.cpp)

add_library(${PLUGIN_NAME} MODULE
    ${ADAPTER_SOURCES}
    ${SRCS}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Gui
    Dtk${DTK_VERSION_MAJOR}::Core
    Dtk${DTK_VERSION_MAJOR}::Gui
    PkgConfig::Gio-2.0
    PkgConfig::GTK3
    PkgConfig::X11
    PkgConfig::XCB
    PkgConfig::OPENSSL
    PkgConfig::FONTCONFIG
)
include(GNUInstallDirs)
install(TARGETS ${PLUGIN_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/deepin-service-manager/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugin-dde-appearance.json DESTINATION share/deepin-service-manager/user/)

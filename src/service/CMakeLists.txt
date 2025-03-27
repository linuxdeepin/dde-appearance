# SPDX-FileCopyrightText: None
#
# SPDX-License-Identifier: CC0-1.0

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-DQT_NO_KEYWORDS)
set(PROJECT_NAME dde-appearance)

find_package(PkgConfig REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core DBus Concurrent Gui)
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED Core Gui)
find_package(KF6WindowSystem REQUIRED)

pkg_check_modules(Gio-2.0 REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET xcursor xfixes x11)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0 gdk-3.0)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb-render xcb xcb-randr xcb-cursor)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(OPENSSL REQUIRED IMPORTED_TARGET openssl)

add_definitions(-DUSE_QT)

qt_add_dbus_adaptor(ADAPTER_SOURCES ../../dbus/org.deepin.dde.Appearance1.xml ./dbus/appearance1.h Appearance1)

set(SRCS
    ./plugin.cpp
    ./dbus/scaleFactors.h
    ./dbus/scaleFactors.cpp
    ./dbus/appearancedbusproxy.h
    ./dbus/appearancedbusproxy.cpp
    ./dbus/appearanceproperty.h
    ./dbus/appearanceproperty.cpp
    ./dbus/appearance1thread.h
    ./dbus/appearance1thread.cpp
    ./dbus/appearance1.h
    ./dbus/appearance1.cpp
    ./impl/appearancemanager.h
    ./impl/appearancemanager.cpp
    ./impl/fsnotify.h
    ./impl/fsnotify.cpp
    ./impl/appearancesyncconfig.h
    ./impl/appearancesyncconfig.cpp
    ./modules/background/background.h
    ./modules/background/background.cpp
    ./modules/fonts/fontsmanager.h
    ./modules/fonts/fontsmanager.cpp
    ./modules/subthemes/subthemes.h
    ./modules/subthemes/subthemes.cpp
    ./modules/subthemes/scanner.h
    ./modules/subthemes/scanner.cpp
    ./modules/subthemes/theme.h
    ./modules/subthemes/theme.cpp
    ./modules/subthemes/customtheme.h
    ./modules/subthemes/customtheme.cpp
    ./modules/api/themes.h
    ./modules/api/themes.cpp
    ./modules/api/keyfile.cpp
    ./modules/api/keyfile.h
    ./modules/api/locale.h
    ./modules/api/locale.cpp
    ./modules/api/themethumb.h
    ./modules/api/themethumb.cpp
    ./modules/api/dfile.h
    ./modules/api/dfile.cpp
    ./modules/api/utils.h
    ./modules/api/utils.cpp
    ./modules/api/syncconfig.h
    ./modules/api/syncconfig.cpp
    ./modules/api/sunrisesunset.h
    ./modules/api/sunrisesunset.cpp
    ./modules/api/compatibleengine.h
    ./modules/api/compatibleengine.cpp
    ./modules/background/format.h
    ./modules/background/format.cpp
    ./modules/background/backgrounds.cpp
    ./modules/background/backgrounds.h
    ./modules/common/commondefine.h
    ./modules/dconfig/dconfigsettings.h
    ./modules/dconfig/dconfigsettings.cpp
    ./modules/dconfig/phasewallpaper.h
    ./modules/dconfig/phasewallpaper.cpp
    ${ADAPTER_SOURCES}
)

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
    KF6::WindowSystem
)
include(GNUInstallDirs)
install(TARGETS ${PLUGIN_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}/deepin-service-manager/)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/plugin-dde-appearance.json DESTINATION share/deepin-service-manager/user/)

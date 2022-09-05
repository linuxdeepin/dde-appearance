set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-DQT_NO_KEYWORDS)

find_package(Qt5 REQUIRED COMPONENTS Core DBus Concurrent Gui)
find_package(DtkCore REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)
pkg_check_modules(DFrameworkDBus REQUIRED IMPORTED_TARGET dframeworkdbus)
pkg_check_modules(Gio-2.0 REQUIRED IMPORTED_TARGET gio-2.0)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET xcursor xfixes x11)
pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0 gdk-3.0)
pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb-render xcb xcb-randr xcb-cursor)
pkg_check_modules(FONTCONFIG REQUIRED IMPORTED_TARGET fontconfig)
pkg_check_modules(OPENSSL REQUIRED IMPORTED_TARGET openssl)


qt5_add_dbus_adaptor(ADAPTER_SOURCES
                     ../../dbus/org.deepin.daemon.Appearance1.xml
                     impl/appearance1.h
                     Appearance1)
add_definitions(-DUSE_QT)

set(SRCS
    ./main.cpp
    ./impl/appearance1.h
    ./impl/appearance1.cpp
    ./impl/appearancemanager.h
    ./impl/appearancemanager.cpp
    ./impl/fsnotify.h
    ./impl/fsnotify.cpp
    ./impl/appearancesyncconfig.h
    ./impl/appearancesyncconfig.cpp
    ./impl/cursorchangehandler.h
    ./impl/cursorchangehandler.cpp
    ./impl/wallpaperscheduler.h
    ./impl/wallpaperscheduler.cpp
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
    ./modules/background/format.h
    ./modules/background/format.cpp
    ./modules/background/backgrounds.cpp
    ./modules/background/backgrounds.h
    ./modules/common/commondefine.h
    ./modules/dconfig/dconfigsettings.h
    ./modules/dconfig/dconfigsettings.cpp
    ./modules/dconfig/phasewallpaper.h
    ./modules/dconfig/phasewallpaper.cpp
    ../../dbus/types/scaleFactors.h
    ../../dbus/types/scaleFactors.cpp
)

add_executable(dde-appearance
    ${SRCS}
    ${ADAPTER_SOURCES}
)


target_include_directories(dde-appearance PUBLIC
    PkgConfig::QGSettings
    PkgConfig::DFrameworkDBus
    PkgConfig::Gio-2.0 
    PkgConfig::GTK3
    PkgConfig::X11
    PkgConfig::XCB
    PkgConfig::OPENSSL
    PkgConfig::FONTCONFIG

)

target_link_libraries(dde-appearance
    Qt5::Core
    Qt5::DBus
    Qt5::Concurrent
    Qt5::Gui
    PkgConfig::QGSettings
    PkgConfig::DFrameworkDBus
    PkgConfig::Gio-2.0 
    PkgConfig::GTK3
    PkgConfig::X11
    PkgConfig::XCB
    PkgConfig::OPENSSL
    PkgConfig::FONTCONFIG
    ${DtkCore_LIBRARIES}
)

install(TARGETS dde-appearance DESTINATION bin)

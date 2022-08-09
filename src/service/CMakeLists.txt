set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-DQT_NO_KEYWORDS)

find_package(Qt5 REQUIRED COMPONENTS Core DBus Concurrent Gui)
find_package(DtkCore REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(QGSettings REQUIRED gsettings-qt)
pkg_check_modules(DFrameworkDBus REQUIRED dframeworkdbus)
pkg_check_modules(Gio-2.0 REQUIRED gio-2.0)
pkg_check_modules(X11 REQUIRED xcursor xfixes x11)
pkg_check_modules(Xcursor REQUIRED xcursor)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0 gdk-3.0)
pkg_check_modules(XCB REQUIRED xcb-render xcb xcb-randr xcb-cursor)
pkg_check_modules(OPENSSL REQUIRED openssl)
pkg_check_modules(FONTCONFIG REQUIRED fontconfig)


qt5_add_dbus_adaptor(ADAPTER_SOURCES
                     ../../DBus/org.deepin.daemon.Appearance1.xml
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
    ../../DBus/types/scaleFactors.h
    ../../DBus/types/scaleFactors.cpp
)

add_executable(dde-appearance
    ${SRCS}
    ${ADAPTER_SOURCES}
)


target_include_directories(dde-appearance PUBLIC
    ${QGSettings_INCLUDE_DIRS}
    ${DFrameworkDBus_INCLUDE_DIRS}
    ${Gio-2.0_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${Xcursor_INCLUDE_DIRS}
    ${XCB_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIRS}

)

target_link_libraries(dde-appearance
    Qt5::Core
    Qt5::DBus
    Qt5::Concurrent
    Qt5::Gui
    ${QGSettings_LIBRARIES}
    ${DFrameworkDBus_LIBRARIES}
    ${Gio-2.0_LIBRARIES}
    ${GTK3_LIBRARIES}
    ${X11_LIBRARY_DIRS}
    ${DtkCore_LIBRARIES}
    ${XCB_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${Xcursor_LIBRARIES}
)

install(TARGETS dde-appearance DESTINATION bin)

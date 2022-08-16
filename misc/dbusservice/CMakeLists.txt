set(DBUS_SERVICE_FILES
    org.deepin.daemon.Appearance1.service
)

install(FILES ${DBUS_SERVICE_FILES} DESTINATION share/dbus-1/services/)

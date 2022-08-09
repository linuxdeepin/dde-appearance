set(SYSTEMD_FILES
    org.deepin.daemon.appearance1.service
)

install(FILES ${SYSTEMD_FILES} DESTINATION lib/systemd/user/)

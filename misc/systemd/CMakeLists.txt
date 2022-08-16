set(SYSTEMD_FILES
    dde-appearance.service
)

install(FILES ${SYSTEMD_FILES} DESTINATION lib/systemd/user/)

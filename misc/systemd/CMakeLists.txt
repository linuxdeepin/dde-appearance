configure_file(
  dde-fakewm.service.in
  dde-fakewm.service
  @ONLY
)

set(SYSTEMD_SERVICE_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/dde-fakewm.service
)

install(FILES ${SYSTEMD_SERVICE_FILES} DESTINATION lib/systemd/user/)
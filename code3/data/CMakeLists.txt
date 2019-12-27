macro(INSTALL_IMAGES SOURCE_PATH DEST_DIR)
	file(GLOB files ${SOURCE_PATH})
	install(FILES ${files} DESTINATION ${CMAKE_C3_RESOURCES_DIR}/images/${DEST_DIR})
endmacro(INSTALL_IMAGES)


INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/fileicons/16px/*.png" "fileicons/16px")
INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/fileicons/32px/*.png" "fileicons/32px")
INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/fileicons/48px/*.png" "fileicons/48px")
INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/fileicons/512px/*.png" "fileicons/512px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/flat/24px/*.png" "flat/24px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/misc/*.png" "misc")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/general/20px/*.png" "general/20px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/externalcommand/20px/*.png" "externalcommand/20px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/dock/12px/*.png" "dock/12px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/completion/16px/*.png" "completion/16px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/new/64px/*.png" "new/64px")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/imageeditor/*.png" "imageeditor")

INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/message/24px/*.png" "message/24px")
INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/message/32px/*.png" "message/32px")
INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/message/40px/*.png" "message/40px")
INSTALL_IMAGES("${CMAKE_CURRENT_SOURCE_DIR}/images/message/128px/*.png" "message/128px")

file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/colors/*.c3colors")
install(FILES ${files} DESTINATION ${CMAKE_C3_RESOURCES_DIR}/colors/)

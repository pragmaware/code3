project(ssex)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

set(PACKAGE "Code3")
set(NICENAME "Code 3")
set(VERSION_MAJOR "1")
set(VERSION_BRANCH "1.0")
set(VERSION_RELEASE "1.0.0")
set(VERSION_CODENAME "Pragma")

if(UNIX)
	if(APPLE)
		set(CMAKE_C3_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/Contents/Resources/")
	else()
		# Assume GNU/Linux
		set(CMAKE_C3_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/share/ssex/${VERSION_BRANCH}/")
	endif()
elseif(WIN32)
	set(CMAKE_C3_RESOURCES_DIR "${CMAKE_INSTALL_PREFIX}/")
endif()

configure_file(${PROJECT_SOURCE_DIR}/cmake/c3_sysconfig.h.cmake ${PROJECT_BINARY_DIR}/c3_sysconfig.h)

add_subdirectory(data)
add_subdirectory(src)

#################################################################################################
# GENERAL CMAKE CONFIGURATION
#################################################################################################

# Set conventional loops
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

# Minimum cmake version
cmake_minimum_required(VERSION 3.0.2)

# CMake policy: Automatically link Qt executables to qtmain.lib target on Windows
cmake_policy(SET CMP0020 NEW)
# CMake policy: Ignore COMPILE_DEFINITIONS_<Config> properties.
cmake_policy(SET CMP0043 NEW)

#################################################################################################
# GENERAL VARIABLES
#################################################################################################

# Install path
if(UNIX)
	if(APPLE)
		# MacOS X
		# Due to the special bundle structure ignore
		# the prefix eventually set by the user.
		set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/Code3.app)
	else()
		# Linux / BSD
		if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
			#fix package build
			if(PREFIX)
				set(CMAKE_INSTALL_PREFIX ${PREFIX})
			else()
				set(CMAKE_INSTALL_PREFIX /usr/local)
			endif()
		endif()
	endif()
elseif(WIN32)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/release)
endif()

IF(APPLE)
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
ENDIF(APPLE)

add_subdirectory(code3tags)
add_subdirectory(code3)

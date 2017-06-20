cmake_minimum_required (VERSION 3.7.2)
project (ikDiscon)

# Version number
set (ikDiscon_VERSION_MAJOR 0)
set (ikDiscon_VERSION_MINOR 0)

# Include directories
set (IKDISCON_INCLUDE_DIRS ${IKDISCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include/)
set (IKDISCON_INCLUDE_DIRS ${IKDISCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/ikClwindconWTCon/)
set (IKDISCON_INCLUDE_DIRS ${IKDISCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/ikClwindconWTConfig/)
set (IKDISCON_INCLUDE_DIRS ${IKDISCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/ikTpman/)
include_directories ("${IKDISCON_INCLUDE_DIRS}")
include_directories ("${PROJECT_BINARY_DIR}")

# ikDiscon source files
set (IKDISCON_SOURCES ${IKDISCON_SOURCES} ${PROJECT_SOURCE_DIR}/src/ikTpman/ikTpman.c)
set (IKDISCON_SOURCES ${IKDISCON_SOURCES} ${PROJECT_SOURCE_DIR}/src/ikClwindconWTConfig/ikClwindconWTConfig.c)
set (IKDISCON_SOURCES ${IKDISCON_SOURCES} ${PROJECT_SOURCE_DIR}/src/ikClwindconWTCon/ikClwindconWTCon.c)
set (IKDISCON_SOURCES ${IKDISCON_SOURCES} ${PROJECT_SOURCE_DIR}/src/discon/discon.c)

# shared ikDiscon library
include (GenerateExportHeader)
add_library (ikDiscon SHARED ${IKDISCON_SOURCES})
GENERATE_EXPORT_HEADER (ikDiscon
	BASE_NAME ikDiscon
	EXPORT_MACRO_NAME ikDiscon_EXPORT
	EXPORT_FILE_NAME ikDiscon_EXPORT.h
	STATIC_DEFINE ikDiscon_BUILT_AS_STATIC
)
target_link_libraries (ikDiscon PRIVATE ${PROJECT_SOURCE_DIR}/ikWitcon.lib)

cmake_minimum_required (VERSION 3.7.2)
project (ikDiscon)

# Version number
set (ikDiscon_VERSION_MAJOR 0)
set (ikDiscon_VERSION_MINOR 0)

# Include directories
include_directories ("${PROJECT_SOURCE_DIR}")
include_directories ("${PROJECT_BINARY_DIR}")

# ikDiscon source files
set (IKDISCON_SOURCES ${IKDISCON_SOURCES} ${PROJECT_SOURCE_DIR}/discon.c)

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

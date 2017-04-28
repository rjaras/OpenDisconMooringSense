cmake_minimum_required (VERSION 3.7.2)
project (ikDiscon)

# Version number
set (ikDiscon_VERSION_MAJOR 0)
set (ikDiscon_VERSION_MINOR 0)

# Controller configuration
set (CONTROLLER_CONFIGURATION "Innwind10MWControllerConfig.h")
configure_file (
	"${PROJECT_SOURCE_DIR}/ikDisconConfig.h.in"
	"${PROJECT_BINARY_DIR}/ikDisconConfig.h"
)
include_directories (
	"${PROJECT_BINARY_DIR}"
)

# ikWitcon include directories
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSlti/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikTfList/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikVfnotch/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikNotchList/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikLutbl/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikLinCon/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikStpgen/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikRegionSelector/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikConLoop/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikTpman/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikPowman/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSsprofile/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSsproc/)
set (IKWITCON_INCLUDE_DIRS ${IKWITCON_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikVpvsController/)
include_directories (
	"${IKWITCON_INCLUDE_DIRS}"
)

# ikWitcon source files
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSlti/ikSlti.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikTfList/ikTfList.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikVfnotch/ikVfnotch.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikNotchList/ikNotchList.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikLutbl/ikLutbl.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikLinCon/ikLinCon.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikStpgen/ikStpgen.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikRegionSelector/ikRegionSelector.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikConLoop/ikConLoop.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikTpman/ikTpman.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikPowman/ikPowman.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSsprofile/ikSsprofile.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSsproc/ikSsproc.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/ikWitcon/source/ikVpvsController/ikVpvsController.c)
set (IKWITCON_SOURCES ${IKWITCON_SOURCES} ${PROJECT_SOURCE_DIR}/discon.c)

# shared library
include (GenerateExportHeader)
add_library (ikDiscon SHARED ${IKWITCON_SOURCES})
GENERATE_EXPORT_HEADER (ikDiscon
	BASE_NAME ikDiscon
	EXPORT_MACRO_NAME ikDiscon_EXPORT
	EXPORT_FILE_NAME ikDiscon_EXPORT.h
	STATIC_DEFINE ikDiscon_BUILT_AS_STATIC
)

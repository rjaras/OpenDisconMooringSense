cmake_minimum_required (VERSION 3.7.2)
project (ikWitcon)

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

# static ikWitcon private library
add_library (ikWitcon STATIC ${IKWITCON_SOURCES})

cmake_minimum_required (VERSION 3.5.1)
project (ikWitcon)

# copy ikDiscon directory to build directory
file( COPY ${PROJECT_SOURCE_DIR}/ikDiscon DESTINATION ${PROJECT_BINARY_DIR}/ )

# copy header files to build directory
file(
	COPY
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikConLoop/ikConLoop.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikLinCon/ikLinCon.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikLutbl/ikLutbl.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikNotchList/ikNotchList.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikRegionSelector/ikRegionSelector.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikSlti/ikSlti.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikStpgen/ikStpgen.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikTfList/ikTfList.h
	${PROJECT_SOURCE_DIR}/ikWitcon/source/ikVfnotch/ikVfnotch.h
	DESTINATION ${PROJECT_BINARY_DIR}/ikDiscon/include/ )
	
# copy doc svgs to build directory
file( GLOB SVG_FILES
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikConLoop*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikLinCon*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikLutbl*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikNotchList*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikRegionSelector*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikSlti*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikStpgen*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikTfList*.svg
	${PROJECT_SOURCE_DIR}/ikWitcon/doc/svg/ikVfnotch*.svg
	)
file( COPY ${SVG_FILES}	DESTINATION ${PROJECT_BINARY_DIR}/ikDiscon/doc/svg/ )

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
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library (ikWitcon STATIC ${IKWITCON_SOURCES})

cmake_minimum_required (VERSION 2.6)

project (roxml)

set (VERSION "2")
set (SOVERSION "2.3.0")

set (TOPDIR "")

include_directories( "${PROJECT_SOURCE_DIR}/${TOPDIR}inc" )

add_definitions (-DIGNORE_EMPTY_TEXT_NODES)

add_library(roxml-dynamic SHARED
		${TOPDIR}src/roxml.c
		${TOPDIR}src/roxml-internal.c
		${TOPDIR}src/roxml-parse-engine.c
		${TOPDIR}src/roxml.h
		${TOPDIR}src/roxml-defines.h
		${TOPDIR}src/roxml-internal.h
		${TOPDIR}src/roxml-parse-engine.h
		${TOPDIR}src/roxml-types.h
		${TOPDIR}src/roxml-tune.h
		${TOPDIR}src/roxml-win32-native.h
		)
		
add_library(roxml-static STATIC
		${TOPDIR}src/roxml.c
		${TOPDIR}src/roxml-internal.c
		${TOPDIR}src/roxml-parse-engine.c
		${TOPDIR}src/roxml.h
		${TOPDIR}src/roxml-defines.h
		${TOPDIR}src/roxml-internal.h
		${TOPDIR}src/roxml-parse-engine.h
		${TOPDIR}src/roxml-types.h
		${TOPDIR}src/roxml-tune.h
		${TOPDIR}src/roxml-win32-native.h
		)
		
add_executable(roxml-tool
		${TOPDIR}src/roxml-parser.c
		)

target_link_libraries(roxml-tool roxml-dynamic)

set_target_properties (roxml-dynamic 
	PROPERTIES 
		OUTPUT_NAME roxml 
		SOVERSION ${SOVERSION} 
		VERSION ${VERSION}
	)
	
set_target_properties (roxml-static 
	PROPERTIES 
		OUTPUT_NAME roxml
	)
	
set_target_properties (roxml-tool 
	PROPERTIES 
		OUTPUT_NAME roxml
	)

install (TARGETS roxml-dynamic roxml-static DESTINATION lib/)
install (TARGETS roxml-tool DESTINATION bin/)
install (FILES "${TOPDIR}src/roxml.h" DESTINATION include/)


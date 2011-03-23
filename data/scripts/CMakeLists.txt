cmake_minimum_required (VERSION 2.6)

project (roxml)

set (VERSION "2")
set (SOVERSION "2.0.2")

include_directories( "${PROJECT_SOURCE_DIR}/../../inc" )

add_definitions (-DIGNORE_EMPTY_TEXT_NODES)

add_library(roxml-dynamic SHARED
		../../src/roxml.c
		../../src/roxml-internal.c
		../../src/roxml-parse-engine.c
		../../inc/roxml.h
		../../inc/roxml-defines.h
		../../inc/roxml-internal.h
		../../inc/roxml-parse-engine.h
		../../inc/roxml-types.h
		)
		
add_library(roxml-static STATIC
		../../src/roxml.c
		../../src/roxml-internal.c
		../../src/roxml-parse-engine.c
		../../inc/roxml.h
		../../inc/roxml-defines.h
		../../inc/roxml-internal.h
		../../inc/roxml-parse-engine.h
		../../inc/roxml-types.h
		)
		
add_executable(roxml-tool
		../../src/roxml-parser.c
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
install (FILES "../../inc/roxml.h" DESTINATION include/)


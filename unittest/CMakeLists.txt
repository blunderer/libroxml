project(unittest C)

include_directories(${CMAKE_SOURCE_DIR}/src)
# global test defines
add_definitions(-DUNITTEST -DREENTRANT -D__DEBUG -DIGNORE_EMPTY_TEXT_NODES)
# additional defines for default configuration
set(DEFAULT_CONFIG "CONFIG_XML_CONTENT;CONFIG_XML_NAV;CONFIG_XML_BUFF;CONFIG_XML_COMMIT;CONFIG_XML_EDIT;CONFIG_XML_FILE;CONFIG_XML_XPATH")
# additional defines for tiny configuration
set(TINY_CONFIG "CONFIG_XML_CONTENT;CONFIG_XML_NAV;CONFIG_XML_BUFF;CONFIG_XML_FILE;CONFIG_XML_HEAP_BUFFERS=1;CONFIG_XML_SMALL_BUFFER=1;CONFIG_XML_SMALL_INPUT_FILE=1;CONFIG_XML_THREAD_SAFE=0")

# create special roxml library for unittests
# which allows to acces internal static functions
set(ROXML_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/roxml_buff.c
    ${CMAKE_SOURCE_DIR}/src/roxml_commit.c
    ${CMAKE_SOURCE_DIR}/src/roxml_content.c
    ${CMAKE_SOURCE_DIR}/src/roxml_core.c
    ${CMAKE_SOURCE_DIR}/src/roxml_edit.c
    ${CMAKE_SOURCE_DIR}/src/roxml_file.c
    ${CMAKE_SOURCE_DIR}/src/roxml_mem.c
    ${CMAKE_SOURCE_DIR}/src/roxml_nav.c
    ${CMAKE_SOURCE_DIR}/src/roxml_parser.c
    ${CMAKE_SOURCE_DIR}/src/roxml_xpath.c
    ${CMAKE_SOURCE_DIR}/src/roxml_stub.c
    )

set(ROXML_TEST_TINY_SOURCES
    ${CMAKE_SOURCE_DIR}/src/roxml_buff.c
    ${CMAKE_SOURCE_DIR}/src/roxml_content.c
    ${CMAKE_SOURCE_DIR}/src/roxml_core.c
    ${CMAKE_SOURCE_DIR}/src/roxml_file.c
    ${CMAKE_SOURCE_DIR}/src/roxml_mem.c
    ${CMAKE_SOURCE_DIR}/src/roxml_nav.c
    ${CMAKE_SOURCE_DIR}/src/roxml_parser.c
    ${CMAKE_SOURCE_DIR}/src/roxml_stub.c
    )

add_library(roxml_test ${ROXML_TEST_SOURCES})
set_target_properties(roxml_test PROPERTIES COMPILE_DEFINITIONS "${DEFAULT_CONFIG}")

# roxml.unitC
add_executable(roxml.unitC unittest.c roxml.test.c)
set_target_properties(roxml.unitC PROPERTIES COMPILE_DEFINITIONS "${DEFAULT_CONFIG}")
target_link_libraries(roxml.unitC roxml_test)
add_test(NAME roxml.unitC
    COMMAND roxml.unitC
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# roxml.perf
add_executable(roxml.perf roxml.perf.c)
set_target_properties(roxml.perf PROPERTIES COMPILE_DEFINITIONS "${DEFAULT_CONFIG}")
target_link_libraries(roxml.perf roxml_test)

# roxml.mem
add_executable(roxml.mem roxml.mem.c)
set_target_properties(roxml.mem PROPERTIES COMPILE_DEFINITIONS "${DEFAULT_CONFIG}")
target_link_libraries(roxml.mem roxml_test)

# library for testing tiny configuration
add_library(roxml_test_tiny ${ROXML_TEST_TINY_SOURCES})
set_target_properties(roxml_test_tiny PROPERTIES COMPILE_DEFINITIONS "${TINY_CONFIG}")

# roxml.tiny.unitC
add_executable(roxml.tiny.unitC unittest.c roxml.test.c)
set_target_properties(roxml.tiny.unitC PROPERTIES COMPILE_DEFINITIONS "${TINY_CONFIG}")
target_link_libraries(roxml.tiny.unitC roxml_test_tiny)
add_test(NAME roxml.tiny.unitC
    COMMAND roxml.tiny.unitC
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

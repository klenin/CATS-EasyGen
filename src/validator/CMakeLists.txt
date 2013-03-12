include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(TOKENIZER_FILENAME ValidatorTokenizer)
find_package(FLEX)
flex_target(tokenizer ${TOKENIZER_FILENAME}.l ${CMAKE_CURRENT_BINARY_DIR}/${TOKENIZER_FILENAME}.c)

set(TESTER_NAME ValidatorTester)
set(TESTER_CMAKE_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/${TESTER_NAME}.cmake)
set(TESTER_SOURCE ${TESTER_NAME}.c)
set(TESTER_TARGET validator-tester)
set(TESTER_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TESTER_TARGET})

file(GLOB SOURCES *.c)
list(REMOVE_ITEM SOURCES ${TESTER_SOURCE})
set(SOURCES ${SOURCES} ${FLEX_tokenizer_OUTPUTS})

set(LIBRARIES allocator exceptions file numerics parser)
set(VALIDATOR_LIBRARY_DEPENDENCIES ${LIBRARIES} PARENT_SCOPE)

if(CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(
        ValidatorMain.c
        PROPERTIES COMPILE_FLAGS -Wno-unused-parameter)
    set_source_files_properties(
        ${FLEX_tokenizer_OUTPUTS}
        PROPERTIES COMPILE_FLAGS -Wno-unused-function)
elseif(MSVC)
    # C4244: http://msdn.microsoft.com/en-us/library/th7a07tz.aspx
    # C4627: http://msdn.microsoft.com/en-us/library/6kck0s93.aspx
    # C4996: http://msdn.microsoft.com/en-us/library/ttcz0bys.aspx
    # C6011: http://msdn.microsoft.com/en-us/library/2ayc37ac.aspx
    # C6385: http://msdn.microsoft.com/en-us/library/ms182088.aspx
    # C28278: http://msdn.microsoft.com/en-us/library/hh739083.aspx
    set_source_files_properties(
        ${FLEX_tokenizer_OUTPUTS}
        PROPERTIES COMPILE_FLAGS "/wd 4244 /wd 4267 /wd 4996 /wd 6011 /wd 6385 /wd 28278")
endif()

add_library(validator STATIC ${SOURCES})
target_link_libraries(validator ${LIBRARIES})

add_executable(${TESTER_TARGET} ${TESTER_SOURCE})
target_link_libraries(validator-tester validator)

set(TESTS_SRC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(TESTS_DST_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests)
add_custom_command(
    TARGET ${TESTER_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ARGS ${TESTS_SRC_DIRECTORY} ${TESTS_DST_DIRECTORY}
)

file(GLOB_RECURSE TEST_SUITES ${TESTS_SRC_DIRECTORY} ${TESTS_SRC_DIRECTORY}/*.inp)
foreach(SUITE ${TEST_SUITES})
    get_filename_component(SUITE_NAME ${SUITE} NAME_WE)
    add_test(
        ${SUITE_NAME}
        ${CMAKE_COMMAND}
        -DSUITE=${SUITE_NAME}
        -DTESTER=${TESTER_EXECUTABLE}
        -DTESTDIR=${TESTS_DST_DIRECTORY}
        -P ${TESTER_CMAKE_SCRIPT})
endforeach()

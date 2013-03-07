include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(TOKENIZER_FILENAME ValidatorTokenizer)
find_package(FLEX)
flex_target(tokenizer ${TOKENIZER_FILENAME}.l ${CMAKE_CURRENT_BINARY_DIR}/${TOKENIZER_FILENAME}.c)

set(TESTER_NAME ValidatorTester)
set(TESTER_CMAKE_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/${TESTER_NAME}.cmake)
set(TESTER_SOURCE ${TESTER_NAME}.c)
set(TESTER_TARGET validator-tester)
set(TESTER_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TESTER_TARGET}.exe)

file(GLOB SOURCES *.c)
list(REMOVE_ITEM SOURCES ${TESTER_SOURCE})
set(SOURCES ${SOURCES} ${FLEX_tokenizer_OUTPUTS})

set(LIBRARIES allocator exceptions file parser)

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

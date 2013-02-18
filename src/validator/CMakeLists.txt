include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(TOKENIZER_FILENAME ValidatorTokenizer)
find_package(FLEX)
flex_target(tokenizer ${TOKENIZER_FILENAME}.l ${CMAKE_CURRENT_BINARY_DIR}/${TOKENIZER_FILENAME}.c)

file(GLOB SOURCES *.c)
set(TESTER_MAIN ValidatorTester.c)
set(SOURCES ${SOURCES} ${FLEX_tokenizer_OUTPUTS})
list(REMOVE_ITEM SOURCES ${TESTER_MAIN})

set(LIBRARIES allocator exceptions file numerics parser)

add_library(validator STATIC ${SOURCES})
add_executable(validator-tester ${TESTER_MAIN})
target_link_libraries(validator ${LIBRARIES})
target_link_libraries(validator-tester validator)

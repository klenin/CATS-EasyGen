include_directories(${PROJECT_SOURCE_DIR}/src/parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(TOKENIZER_FILENAME ValidatorTokenizer)
find_package(FLEX)
flex_target(tokenizer ${TOKENIZER_FILENAME}.l ${CMAKE_CURRENT_BINARY_DIR}/${TOKENIZER_FILENAME}.c)

file(GLOB SOURCES *.c)
add_executable(validator ${SOURCES} ${FLEX_tokenizer_OUTPUTS})
target_link_libraries(validator allocator)
target_link_libraries(validator exceptions)
target_link_libraries(validator file)
target_link_libraries(validator parser)

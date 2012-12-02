include_directories(${PROJECT_SOURCE_DIR}/src/parser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(FLEX)
flex_target(tokenizer tokenizer.l ${CMAKE_CURRENT_BINARY_DIR}/tokenizer.c)

file(GLOB SOURCES *.c)
add_executable(validator ${SOURCES} ${FLEX_tokenizer_OUTPUTS})
target_link_libraries(validator parser)
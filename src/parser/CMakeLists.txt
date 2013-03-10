file(GLOB SOURCES *.c)
add_library(parser STATIC ${SOURCES})

set(LIBRARIES allocator exceptions random)
set(PARSER_LIBRARY_DEPENDENCIES ${LIBRARIES} PARENT_SCOPE)
target_link_libraries(parser ${LIBRARIES})

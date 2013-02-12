file(GLOB SOURCES *.c)
add_library(parser STATIC ${SOURCES})

target_link_libraries(parser random)

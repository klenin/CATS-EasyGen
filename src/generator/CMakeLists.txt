include_directories(${PROJECT_SOURCE_DIR}/src/parser)

add_library(generator STATIC easygen.cpp)

target_link_libraries(generator random)
target_link_libraries(generator parser)

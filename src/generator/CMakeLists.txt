add_library(generator STATIC easygen.cpp)

target_link_libraries(generator random)
target_link_libraries(generator parser)

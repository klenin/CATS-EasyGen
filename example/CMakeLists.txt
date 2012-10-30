include_directories(${PROJECT_SOURCE_DIR}/parser)
include_directories(${PROJECT_SOURCE_DIR}/generator)

link_directories(${PROJECT_BINARY_DIR}/generator)

add_executable(samplegen samplegen.cpp)

target_link_libraries(samplegen generator)

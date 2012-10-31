cmake_minimum_required(VERSION 2.6)

project(EASYGEN)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(GCC_FLAGS -W -Wall -pedantic -Wextra -Weffc++ -Winit-self
              -Woverloaded-virtual -Wnon-virtual-dtor
              -Wold-style-cast -Wconversion -Wsign-conversion
)

if(CMAKE_COMPILER_IS_GNUCC)
    foreach(flag ${GCC_FLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach(flag)
endif(CMAKE_COMPILER_IS_GNUCC)

add_subdirectory(src/parser)
add_subdirectory(src/generator)
add_subdirectory(src/example)

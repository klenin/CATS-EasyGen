cmake_minimum_required(VERSION 2.6)

project(EASYGEN)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

SET (GCC_FLAGS -Wall -pedantic -Wextra -Weffc++ -Winit-self
               -Woverloaded-virtual -Wnon-virtual-dtor
               -Wold-style-cast -Wconversion -Wsign-conversion
    )
SET (VC_FLAGS /Wall)

if(CMAKE_COMPILER_IS_GNUCC)
    set(ADDITIONAL_CXX_FLAGS ${GCC_FLAGS})
elseif(MSVC)
    set(ADDITIONAL_CXX_FLAGS ${VC_FLAGS})
endif(CMAKE_COMPILER_IS_GNUCC)

foreach(flag ${ADDITIONAL_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
endforeach(flag)

add_subdirectory(parser)
add_subdirectory(generator)
add_subdirectory(example)

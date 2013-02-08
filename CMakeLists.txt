cmake_minimum_required(VERSION 2.6)

project(EASYGEN)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
message(SEND_ERROR "In-source builds are not allowed.")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(GCC_COMMON_FLAGS -W -Wall -pedantic -Wextra -Winit-self)
set(GCC_CFLAGS -std=c99)
set(GCC_CXXFLAGS -Weffc++ -Woverloaded-virtual -Wnon-virtual-dtor -Wold-style-cast)
set(VC_FLAGS /analyze /W3 /D_CRT_SECURE_NO_WARNINGS)

if(MSVC)
    add_definitions(${VC_FLAGS})
elseif(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(${GCC_COMMON_FLAGS})
    foreach(flag ${GCC_CFLAGS})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
    endforeach()
    foreach(flag ${GCC_CXXFLAGS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
    endforeach()
endif()

add_subdirectory(src/libraries)
add_subdirectory(src/parser)
add_subdirectory(src/validator)
add_subdirectory(src/generator)
add_subdirectory(src/example)

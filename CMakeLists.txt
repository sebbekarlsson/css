cmake_minimum_required(VERSION 3.20)

project(css)


FILE(GLOB sources src/*.c)
FILE(GLOB lib_sources src/*.c)

add_compile_options(-g -Wall -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(css_e ${sources})

list(FILTER sources EXCLUDE REGEX "main.c")
add_library(css SHARED ${lib_sources})

# target_sources(css_l PUBLIC ${sources})

set_target_properties(css PROPERTIES PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/css.h)

add_compile_options(-g -Wall -fPIC)

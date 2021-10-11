cmake_minimum_required(VERSION 3.20)


project(css)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tests DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB css_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)


add_executable(css_e ${css_sources})


list(FILTER css_ources EXCLUDE REGEX "main.c")

add_library(css SHARED ${css_sources})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  css
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  css_e
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


include(FetchContent)
FetchContent_Declare(
  hashmap
  URL /home/ianertson/workspace/hashmap
  # GIT_REPOSITORY https://github.com/sebbekarlsson/hashmap.git
  # GIT_TAG master
)
FetchContent_MakeAvailable(hashmap)

target_link_libraries(css PUBLIC hashmap)
target_link_libraries(css_e PUBLIC hashmap)

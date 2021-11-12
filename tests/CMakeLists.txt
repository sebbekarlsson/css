cmake_minimum_required(VERSION 3.20)

project(css_test)


file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


add_executable(css_test src/main.c)


target_link_libraries(css_test PRIVATE css_static)

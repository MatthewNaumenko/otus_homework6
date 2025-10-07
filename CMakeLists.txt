cmake_minimum_required(VERSION 3.16)

project(sparse_matrix
  VERSION 1.0.0
  DESCRIPTION "Infinite sparse 2D matrix with Proxy pattern"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_WARNINGS "Enable extra warnings" ON)

add_executable(sparse_matrix_demo
  src/main.cpp
)
target_include_directories(sparse_matrix_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(ENABLE_WARNINGS)
  if(MSVC)
    target_compile_options(sparse_matrix_demo PRIVATE /W4 /permissive-)
  else()
    target_compile_options(sparse_matrix_demo PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endif()

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating Doxygen documentation"
    VERBATIM
  )
endif()

install(TARGETS sparse_matrix_demo RUNTIME DESTINATION bin)

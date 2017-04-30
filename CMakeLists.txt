cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0048 NEW)

project(imagebm VERSION "0.0.1")

macro(use_c11)
  if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set(CMAKE_C_FLAGS "-std=c11 ${CMAKE_C_FLAGS}")
    endif()
  else()
    set(CMAKE_C_STANDARD 11)
endif()
endmacro(use_c11)

file(GLOB ${PROJECT_NAME}_headers ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB ${PROJECT_NAME}_sources ${PROJECT_SOURCE_DIR}/*.c)

set(TINYCTHREAD_DISABLE_TESTS OFF)
add_subdirectory(tinycthread)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_sources} ${${PROJECT_NAME}_headers} README.md)
add_dependencies(${PROJECT_NAME} tinycthread)
target_link_libraries(${PROJECT_NAME} tinycthread)


cmake_minimum_required(VERSION 3.10)

project(PC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(PC
  main.cpp
  blockage_diag.cpp
  crop_box_filter.cpp
)

target_include_directories(PC PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  target_compile_options(PC PRIVATE /W4 /permissive-)
else()
  target_compile_options(PC PRIVATE -O2 -Wall -Wextra -Wpedantic)
endif()

cmake_minimum_required(VERSION 3.10)
project(tasks_4_plus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task4_mixins 4_mixins.cpp)
add_executable(task5_singleton_log 5_singleton_log.cpp)
add_executable(task6_builder_checkpoints 6_builder_checkpoints.cpp)
add_executable(task7_bridge_set 7_bridge_set.cpp)
add_executable(task8_expression_composite_flyweight 8_expression_composite_flyweight.cpp)

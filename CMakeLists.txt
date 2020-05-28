cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

project(result-cpp LANGUAGES CXX)

add_library(result INTERFACE)
target_include_directories(
  result
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


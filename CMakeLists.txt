cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(result-cpp LANGUAGES CXX)

add_library(result INTERFACE)
target_include_directories(
  result
  INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)


# Tests
find_package(Catch2 REQUIRED)
add_executable(tests tests/ResultTests.cpp)
target_link_libraries(tests result Catch2::Catch2)

include(CTest)
include(Catch)
catch_discover_tests(tests)

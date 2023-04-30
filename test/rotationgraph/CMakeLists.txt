cmake_minimum_required(VERSION 3.22) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(testrotationgraph)

include_directories(
    ../../include
    ../../lithium/dep/glm
)

file(GLOB SOURCES
    testrotationgraph.cpp
)

add_executable(testrotationgraph ${SOURCES})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install(TARGETS testrotationgraph DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
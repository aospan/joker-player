cmake_minimum_required(VERSION 3.0.2)
cmake_policy(VERSION 3.0.2)

project(JokerTV)

exec_program(
    "git"
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "describe --long --always"
    OUTPUT_VARIABLE GIT_VERSION )

add_definitions(-DGIT_VERSION="${GIT_VERSION}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)

# CMakeLists files in this project can
# refer to the root source directory of the project as ${EIKOS_SOURCE_DIR} and
# to the root binary directory of the project as ${EIKOS_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (algopattern)

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic")

# put binaries in the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Dump used compiler flags.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(.)

enable_testing()

set(repeat 10)
# set(repeat 2000)
file(WRITE ${CMAKE_BINARY_DIR}/test.sh "$1; for i in $(seq ${repeat}); do $1 > /dev/null; done\n")


add_executable(strategy strategy.cpp)
add_test(NAME strategy COMMAND sh ${CMAKE_BINARY_DIR}/test.sh $<TARGET_FILE:strategy>)

add_executable(functional functional.cpp)
add_test(NAME functional COMMAND sh ${CMAKE_BINARY_DIR}/test.sh $<TARGET_FILE:functional>)

add_executable(policies policies.cpp)
add_test(NAME policies COMMAND sh ${CMAKE_BINARY_DIR}/test.sh $<TARGET_FILE:policies>)

add_executable(crtp crtp.cpp)
add_test(NAME crtp COMMAND sh ${CMAKE_BINARY_DIR}/test.sh $<TARGET_FILE:crtp>)


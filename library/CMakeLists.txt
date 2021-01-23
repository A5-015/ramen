cmake_minimum_required (VERSION 2.6)
project (ramen)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.42.0 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

FILE(GLOB TESTFILES test/**/catch_*.cpp)
foreach(TESTFILE ${TESTFILES})
    get_filename_component(NAME ${TESTFILE} NAME_WE) 
    add_executable(${NAME} ${TESTFILE} test/catch/fake_serial.cpp)
    target_include_directories(${NAME} PUBLIC src/ test/include/ test/catch/ test/ArduinoJson/src/ test/TaskScheduler/src test/painlessMesh/src)
endforeach()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

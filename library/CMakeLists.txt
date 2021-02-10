cmake_minimum_required(VERSION 3.5)
project(ramen_catch)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_definitions(-D _RAMEN_CATCH_TESTING_)

file(GLOB TESTFILES test/**/catch_*.cpp)
foreach(TESTFILE ${TESTFILES})
    get_filename_component(NAME ${TESTFILE} NAME_WE) 

    add_executable(${NAME} ${TESTFILE} "${PROJECT_BINARY_DIR}/test/include/fake_serial.cpp"
                                       "${PROJECT_BINARY_DIR}/test/include/scheduler.cpp"
                                       "${PROJECT_BINARY_DIR}/src/ramen/logger.hpp"
                                       "${PROJECT_BINARY_DIR}/src/ramen/data_queue.cpp"
                                       "${PROJECT_BINARY_DIR}/src/ramen/log_holder.cpp"
                                       "${PROJECT_BINARY_DIR}/src/ramen/server.cpp")

    target_include_directories(${NAME} PUBLIC "${PROJECT_BINARY_DIR}/src/"
                                              "${PROJECT_BINARY_DIR}/src/ramen/"
                                              "${PROJECT_BINARY_DIR}/test/"
                                              "${PROJECT_BINARY_DIR}/test/include/"
                                              "${PROJECT_BINARY_DIR}/test/TaskScheduler/src/"
                                              "${PROJECT_BINARY_DIR}/test/ArduinoJson/src/")
endforeach()

# -Wall -Wextra -Woverflow
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Werror")
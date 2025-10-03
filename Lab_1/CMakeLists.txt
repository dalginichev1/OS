cmake_minimum_required(VERSION 3.14)

project(Lab_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(main
    src/main.cpp
    src/process.cpp
    src/pipe.cpp
    src/childProcess.cpp
)

add_executable(child
    src/child_main.cpp
    src/childProcessor.cpp
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(child PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WIN32)
    target_link_libraries(main PRIVATE ws2_32)
    target_link_libraries(child PRIVATE ws2_32)
endif()

set_target_properties(child PROPERTIES OUTPUT_NAME "child${CMAKE_EXECUTABLE_SUFFIX}")
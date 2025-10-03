cmake_minimum_required(VERSION 3.14)

project(LabWork)
project(Lab_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(parent
    parent/main.cpp
    parent/process.cpp
    parent/childProcess.cpp
    parent/pipe.cpp
)

add_executable(child
    child/child_main.cpp
    child/childProcessor.cpp
)

if(WIN32)
    target_link_libraries(parent)
    target_link_libraries(child)
else()
    target_link_libraries(parent)
    target_link_libraries(child)
endif()
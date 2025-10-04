cmake_minimum_required(VERSION 3.14)

project(Lab_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(parent
    src/main.cpp
    src/process.cpp
    src/pipe.cpp
    src/childProcess.cpp
)

add_executable(child
    src/child_main.cpp
    src/childProcessor.cpp
)

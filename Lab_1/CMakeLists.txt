cmake_minimum_required(VERSION 3.14)

project(LabWork)
project(Lab_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(main
    src/main.cpp
    src/process.cpp
    src/pipe.cpp
    src/childProcess.cpp
    src/childProcessor.cpp
)
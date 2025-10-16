cmake_minimum_required(VERSION 3.14)

project(Lab_2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ОПТИМИЗАЦИЯ ПРОИЗВОДИТЕЛЬНОСТИ - ДОБАВЬТЕ ЭТИ СТРОКИ!
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    message(STATUS "Build type: Debug")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -DNDEBUG")
    message(STATUS "Build type: Release")
endif()

# ОБЯЗАТЕЛЬНЫЕ ФЛАГИ ДЛЯ PTHREAD
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

include_directories(include)

add_executable(quicksort
    src/main.cpp
    src/quicksort.cpp
    src/thread.cpp
)

# СВЯЗЫВАНИЕ С PTHREAD
target_link_libraries(quicksort pthread)

# ДОПОЛНИТЕЛЬНЫЕ ОПТИМИЗАЦИИ
target_compile_definitions(quicksort PRIVATE _REENTRANT)
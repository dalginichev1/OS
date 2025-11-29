cmake_minimum_required(VERSION 3.10)
project(os_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Абсолютный путь к include директории
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

#
# ----------- libreal1.so -----------
#
add_library(real1 SHARED
    lib_1/Derivative_1.cpp
    lib_1/BubbleSort.cpp
    lib_1/fact1.cpp
)
set_target_properties(real1 PROPERTIES OUTPUT_NAME "libreal1")
target_compile_options(real1 PRIVATE -fvisibility=hidden)
# Абсолютные пути для библиотеки 1
target_include_directories(real1 PUBLIC ${INCLUDE_DIR})

#
# ----------- libreal2.so -----------
#
add_library(real2 SHARED
    lib_2/Derivative_2.cpp
    lib_2/QuickSort.cpp
    lib_2/fact2.cpp
)
set_target_properties(real2 PROPERTIES OUTPUT_NAME "libreal2")
target_compile_options(real2 PRIVATE -fvisibility=hidden)
# Абсолютные пути для библиотеки 2
target_include_directories(real2 PUBLIC ${INCLUDE_DIR})

#
# ----------- program1 (относительные пути) -----------
#
add_executable(program1 program_1/main.cpp)
target_link_libraries(program1 PRIVATE real1)
# ОТНОСИТЕЛЬНЫЕ пути для program1
target_include_directories(program1 PRIVATE ../include)

#
# ----------- program2 (абсолютные пути) -----------
#
add_executable(program2
    program_2/main.cpp
    program_2/DynamicLoader.cpp
)
target_link_libraries(program2 PRIVATE dl)
# Абсолютные пути для program2
target_include_directories(program2 PRIVATE ${INCLUDE_DIR})
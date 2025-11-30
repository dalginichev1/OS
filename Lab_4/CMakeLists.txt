cmake_minimum_required(VERSION 3.14)
project(os_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 21)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)  

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

add_library(contract1 SHARED
    lib_1/Derivative_1.cpp
    lib_1/BubbleSort.cpp
    lib_1/fact1.cpp
)
set_target_properties(contract1 PROPERTIES 
    OUTPUT_NAME "contract1"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_include_directories(contract1 PUBLIC ${INCLUDE_DIR})

add_library(contract2 SHARED
    lib_2/Derivative_2.cpp
    lib_2/QuickSort.cpp
    lib_2/fact2.cpp
)
set_target_properties(contract2 PROPERTIES 
    OUTPUT_NAME "contract2"
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_include_directories(contract2 PUBLIC ${INCLUDE_DIR})

add_executable(program1 program_1/main.cpp)
target_link_libraries(program1 PRIVATE contract1)
target_include_directories(program1 PRIVATE ${INCLUDE_DIR})

add_executable(program2
    program_2/main.cpp
    program_2/DynamicLoader.cpp
)

add_custom_command(TARGET program2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:contract1>
        $<TARGET_FILE_DIR:program2>/libcontract1.so
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:contract2>
        $<TARGET_FILE_DIR:program2>/libcontract2.so
    COMMENT "Copying libraries to program2 directory"
)

target_link_libraries(program2 PRIVATE dl)
target_include_directories(program2 PRIVATE ${INCLUDE_DIR})
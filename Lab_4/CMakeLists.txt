cmake_minimum_required(VERSION 3.10)
project(os_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Абсолютный путь к include директории
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

#
# ----------- libcontract1.so -----------
#
add_library(contract1 SHARED
    lib_1/Derivative_1.cpp
    lib_1/BubbleSort.cpp
    lib_1/fact1.cpp
)
set_target_properties(contract1 PROPERTIES 
    OUTPUT_NAME "contract1"  # Будет libcontract1.so
)
target_include_directories(contract1 PUBLIC ${INCLUDE_DIR})

#
# ----------- libcontract2.so -----------
#
add_library(contract2 SHARED
    lib_2/Derivative_2.cpp
    lib_2/QuickSort.cpp
    lib_2/fact2.cpp
)
set_target_properties(contract2 PROPERTIES 
    OUTPUT_NAME "contract2"  # Будет libcontract2.so
)
target_include_directories(contract2 PUBLIC ${INCLUDE_DIR})

#
# ----------- program1 -----------
#
add_executable(program1 program_1/main.cpp)
target_link_libraries(program1 PRIVATE contract1)
target_include_directories(program1 PRIVATE ${INCLUDE_DIR})

#
# ----------- program2 -----------
#
add_executable(program2
    program_2/main.cpp
    program_2/DynamicLoader.cpp
)

# Копируем библиотеки в ту же директорию, что и program2
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
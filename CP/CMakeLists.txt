cmake_minimum_required(VERSION 3.10)
project(battleship VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опции компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

include_directories(
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/server/include
    ${CMAKE_SOURCE_DIR}/client/include
)

# Серверная часть
add_executable(battleship_server
    server/src/server.cpp
    server/src/server_main.cpp
    server/src/game.cpp
    server/src/player.cpp
    server/src/mutex.cpp
)

target_include_directories(battleship_server PRIVATE
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/server/include
)

# Клиентская часть
add_executable(battleship_client
    client/src/client.cpp
    client/src/client_main.cpp
    client/src/gameBoard.cpp
    client/src/pipeClient.cpp
)

target_include_directories(battleship_client PRIVATE
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/client/include
)
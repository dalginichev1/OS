#pragma once

namespace Constants {
    const int BOARD_SIZE = 10;
    const int MAX_SHIPS = 10;
    const int MAX_PLAYERS = 100;
    const int MAX_GAMES = 50;
    const int BUFFER_SIZE = 1024;
    const int PIPE_TIMEOUT_MS = 100;
    
    const int SHIP_TYPES[5][2] = {
        {1, 4},  
        {2, 3},  
        {3, 2},  
        {4, 1},  
        {5, 1}   
    };

    enum CellStatus {
        EMPTY = 0,
        SHIP = 1,
        HIT = 2,
        MISS = 3,
        SUNK = 4
    };
}
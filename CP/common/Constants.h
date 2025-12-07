#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    const int BOARD_SIZE = 10;
    const int MAX_SHIPS = 10;
    const int MAX_PLAYERS = 100;
    const int MAX_GAMES = 50;
    const int BUFFER_SIZE = 1024;
    const int PIPE_TIMEOUT_MS = 100;
    
    // Типы кораблей и их количество
    const int SHIP_TYPES[5][2] = {
        {1, 4},  // 1 палуба × 4 корабля
        {2, 3},  // 2 палубы × 3 корабля
        {3, 2},  // 3 палубы × 2 корабля
        {4, 1},  // 4 палубы × 1 корабль
        {5, 1}   // 5 палуб × 1 корабль
    };
    
    // Статус клетки
    enum CellStatus {
        EMPTY = 0,
        SHIP = 1,
        HIT = 2,
        MISS = 3,
        SUNK = 4
    };
}

#endif
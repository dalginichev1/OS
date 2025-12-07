#pragma once

#include <vector>
#include <string>
#include "Constants.h"

class GameBoard
{
    private:
        std::vector<std::vector<int>> board;
        std::vector<std::vector<bool>> ships;
        int remainingShips;
        int shipsPlaced;

        bool isValidPlacement(int x, int y, int size, bool horizontal) const;
        void markSunkShip(int x, int y);

    public:
        GameBoard();

        void initialize();
        bool placeShip(int x, int y, int size, bool horizontal);
        bool makeShot(int x, int y);
        bool isShipSunk(int x, int y) const;
        bool allShipsSunk() const;
        bool allShipsPlaced() const;
        
        std::string toString(bool showShips = false) const;
        int getCellStatus(int x, int y) const;
        
        void setCellStatus(int x, int y, int status);
        
        int getRemainingShips() const { return remainingShips; }
        int getShipsPlaced() const { return shipsPlaced; }
        void resetShipsPlaced() { shipsPlaced = 0; }
};
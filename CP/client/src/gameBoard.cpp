#include "GameBoard.h"
#include <sstream>
#include <iostream>

GameBoard::GameBoard() : remainingShips(0), shipsPlaced(0)
{
    initialize();
}

void GameBoard::initialize()
{
    board.resize(Constants::BOARD_SIZE, std::vector<int>(Constants::BOARD_SIZE, Constants::EMPTY));
    ships.resize(Constants::BOARD_SIZE, std::vector<bool>(Constants::BOARD_SIZE, false));
    remainingShips = 0;
    shipsPlaced = 0;

    int i = 0;
    for (i = 0; i < 5; ++i)
    {
        remainingShips += Constants::SHIP_TYPES[i][1];
    }
}

bool GameBoard::placeShip(int x, int y, int size, bool horizontal)
{
    if (!isValidPlacement(x, y, size, horizontal)) return false;

    if (horizontal)
    {
        int i = 0;
        for (i = 0; i < size; ++i)
        {
            ships[x][y + i] = true;
            board[x][y + i] = Constants::SHIP;
        }
    }
    else
    {
        int i = 0;
        for (i = 0; i < size; ++i)
        {
            ships[x + i][y] = true;
            board[x + i][y] = Constants::SHIP;
        }
    }

    ++shipsPlaced;
    return true;
}

bool GameBoard::makeShot(int x, int y)
{
    if (x < 0 || x >= Constants::BOARD_SIZE || y < 0 || y >= Constants::BOARD_SIZE) return false;

    if (board[x][y] == Constants::HIT || board[x][y] == Constants::MISS) return false;

    if (ships[x][y])
    {
        board[x][y] = Constants::HIT;
        if (isShipSunk(x, y))
        {
            markSunkShip(x, y);
            --remainingShips;
        }
        return true;
    }
    else
    {
        board[x][y] = Constants::MISS;
        return false;
    }
}

bool GameBoard::isShipSunk(int x, int y) const
{
    if (!ships[x][y]) return false;

    std::vector<std::pair<int, int>> shipCells;
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> visited(Constants::BOARD_SIZE, std::vector<bool>(Constants::BOARD_SIZE, false));

    stack.push_back(std::make_pair(x, y));

    while (!stack.empty())
    {
        int cx = stack.back().first;
        int cy = stack.back().second;
        stack.pop_back();

        if (cx < 0 || cx >= Constants::BOARD_SIZE || cy < 0 || cy >= Constants::BOARD_SIZE || visited[cx][cy] || !ships[cx][cy]) continue;

        visited[cx][cy] = true;
        shipCells.push_back(std::make_pair(cx, cy));

        stack.push_back(std::make_pair(cx + 1, cy));
        stack.push_back(std::make_pair(cx - 1, cy));
        stack.push_back(std::make_pair(cx, cy + 1));
        stack.push_back(std::make_pair(cx, cy - 1));
    }

    size_t i = 0;
    for (i = 0; i < shipCells.size(); ++i)
    {
        int cx = shipCells[i].first;
        int cy = shipCells[i].second;
        if (board[cx][cy] != Constants::HIT) return false;
    }

    return true;
}

void GameBoard::markSunkShip(int x, int y)
{
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> visited(Constants::BOARD_SIZE, std::vector<bool>(Constants::BOARD_SIZE, false));

    stack.push_back(std::make_pair(x, y));

     while (!stack.empty()) {
        int cx = stack.back().first;
        int cy = stack.back().second;
        stack.pop_back();
        
        if (cx < 0 || cx >= Constants::BOARD_SIZE || cy < 0 || cy >= Constants::BOARD_SIZE || visited[cx][cy] || !ships[cx][cy]) continue;
        
        visited[cx][cy] = true;
        board[cx][cy] = Constants::SUNK;
        
        stack.push_back(std::make_pair(cx + 1, cy));
        stack.push_back(std::make_pair(cx - 1, cy));
        stack.push_back(std::make_pair(cx, cy + 1));
        stack.push_back(std::make_pair(cx, cy - 1));
    }
}

bool GameBoard::allShipsSunk() const
{
    return remainingShips == 0;
}

bool GameBoard::allShipsPlaced() const
{
    int requiredShips = 0;
    int i = 0;
    for (i = 0; i < 5; ++i)
    {
        requiredShips += Constants::SHIP_TYPES[i][1];
    }

    return shipsPlaced == requiredShips;
}

std::string GameBoard::toString(bool showShips) const
{
    std::stringstream ss;

    int i = 0;
    int j = 0;
    ss << " ";
    for (i = 0; i < Constants::BOARD_SIZE; ++i)
    {
        ss << i << " ";
    }
    ss << std::endl;

    for (i = 0; i < Constants::BOARD_SIZE; ++i)
    {
        ss << i << " ";
        for (j = 0; i < Constants::BOARD_SIZE; ++j)
        {
            if(showShips && ships[i][j] && board[i][j] == Constants::EMPTY) ss << "S";
            else
            {
                switch (board[i][j])
                {
                    case Constants::EMPTY: ss << ". "; break;
                    case Constants::SHIP: ss << "S "; break;
                    case Constants::HIT: ss << "X "; break;
                    case Constants::MISS: ss << "O "; break;
                    case Constants::SUNK: ss << "# "; break;
                    default: ss << "? ";
                }
            }
        }
        ss << std::endl;
    }

    return ss.str();
}

int GameBoard::getCellStatus(int x, int y) const
{
    if (x < 0 || x >= Constants::BOARD_SIZE || y < 0 || y >= Constants::BOARD_SIZE) return Constants::EMPTY;

    return board[x][y];
}

void GameBoard::setCellStatus(int x, int y, int status)
{
    if (x >= 0 && x < Constants::BOARD_SIZE && y >= 0 && y < Constants::BOARD_SIZE) board[x][y] = status;
}

bool GameBoard::isValidPlacement(int x, int y, int size, bool horizontal) const
{
    if (horizontal)
    {
        if (y + size > Constants::BOARD_SIZE) return false;

        int i = 0;
        int dx = 0;
        int dy = 0;
        for (i = 0; i < size; ++i)
        {
            if (ships[x][y + i]) return false;
            for (dx = -1; dx <= 1; ++dx)
            {
                for (dy = -1; dy <= 1; ++dy)
                {
                    int nx = x + dx;
                    int ny = y + i + dy;
                    if (nx >= 0 && nx < Constants::BOARD_SIZE && ny >= 0 && ny < Constants::BOARD_SIZE && ships[nx][ny]) return false;
                }
            }
        }
    }
    else
    {
        if (x + size > Constants::BOARD_SIZE) return false;

        int i = 0;
        int dx = 0;
        int dy = 0;
        for (i = 0; i < size; ++i)
        {
            if (ships[x + i][y]) return false;
            for (dx = -1; dx <= 1; ++dx)
            {
                for (dy = -1; dy <= 1; ++dy)
                {
                    int nx = x + dx;
                    int ny = y + i + dy;
                    if (nx >= 0 && nx < Constants::BOARD_SIZE && ny >= 0 && ny < Constants::BOARD_SIZE && ships[nx][ny]) return false;
                }
            }
        }
    }

    return true;
}
#include "Game.h"

#include <algorithm>
#include <iostream>
#include <sstream>

Game::Game(const std::string& name, int id, Player* creator)
    : name(name), gameId(id), player1(creator), player2(nullptr), isActive(true),
      currentTurn(nullptr), player1Ready(false), player2Ready(false), remainingShips1(0),
      remainingShips2(0) 
{
    board1.resize(Constants::BOARD_SIZE, std::vector<int>(Constants::BOARD_SIZE, Constants::EMPTY));
    board2.resize(Constants::BOARD_SIZE, std::vector<int>(Constants::BOARD_SIZE, Constants::EMPTY));
    ships1.resize(Constants::BOARD_SIZE, std::vector<bool>(Constants::BOARD_SIZE, false));
    ships2.resize(Constants::BOARD_SIZE, std::vector<bool>(Constants::BOARD_SIZE, false));
}

bool Game::joinGame(Player* player) {
    LockGuard lock(gameMutex);
    if (player2 == nullptr && player1 != player2) {
        player2 = player;
        return true;
    }
    return false;
}

bool Game::isPlayerInGame(Player* player) const {
    LockGuard lock(gameMutex);
    return player == player1 || player == player2;
}

Player* Game::getOpponent(Player* player) const {
    LockGuard lock(gameMutex);
    if (player == player1)
        return player2;
    if (player == player2)
        return player1;
    return nullptr;
}

bool Game::makeMove(Player* player, int x, int y) {
    LockGuard lock(gameMutex);

    if (x < 0 || x >= Constants::BOARD_SIZE || y < 0 || y >= Constants::BOARD_SIZE)
        return false;

    Player* opponent = getOpponent(player);
    if (!opponent || currentTurn != player)
        return false;

    std::vector<std::vector<int>>& targetBoard = (player == player1) ? board2 : board1;
    std::vector<std::vector<bool>>& targetShips = (player == player1) ? ships2 : ships1;

    if (targetBoard[x][y] != Constants::BOARD_SIZE && targetBoard[x][y] != Constants::SHIP)
        return false;

    bool hit = targetBoard[x][y];

    if (hit) {
        targetBoard[x][y] = Constants::HIT;
        if (checkSunk(x, y, targetShips, targetBoard)) {
            markSunkShip(x, y, targetBoard, targetShips);
            if (player == player1) {
                --remainingShips2;
            } else {
                --remainingShips1;
            }
        }
    } else {
        targetBoard[x][y] = Constants::MISS;
    }

    currentTurn = opponent;
    return hit;
}

bool Game::checkSunk(int x, int y, const std::vector<std::vector<bool>>& ships,
                     const std::vector<std::vector<int>>& board) const {
    std::vector<std::pair<int, int>> shipCells;
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> visited(Constants::BOARD_SIZE,
                                           std::vector<bool>(Constants::BOARD_SIZE, false));

    stack.push_back(std::make_pair(x, y));

    while (!stack.empty()) {
        int cx = stack.back().first;
        int cy = stack.back().second;
        stack.pop_back();

        if (cx < 0 || cx >= Constants::BOARD_SIZE || cy < 0 || cy >= Constants::BOARD_SIZE ||
            visited[cx][cy] || !ships[cx][cy])
            continue;

        visited[cx][cy] = true;
        shipCells.push_back(std::make_pair(cx, cy));

        stack.push_back(std::make_pair(cx + 1, cy));
        stack.push_back(std::make_pair(cx - 1, cy));
        stack.push_back(std::make_pair(cx, cy + 1));
        stack.push_back(std::make_pair(cx, cy - 1));
    }

    for (size_t i = 0; i < shipCells.size(); ++i) {
        int cx = shipCells[i].first;
        int cy = shipCells[i].second;
        if (board[cx][cy] != Constants::HIT)
            return false;
    }

    return true;
}

void Game::markSunkShip(int x, int y, std::vector<std::vector<int>>& board,
                        const std::vector<std::vector<bool>>& ships) const {
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> visited(Constants::BOARD_SIZE,
                                           std::vector<bool>(Constants::BOARD_SIZE, false));

    stack.push_back(std::make_pair(x, y));

    while (!stack.empty()) {
        int cx = stack.back().first;
        int cy = stack.back().second;
        stack.pop_back();

        if (cx < 0 || cx >= Constants::BOARD_SIZE || cy < 0 || cy >= Constants::BOARD_SIZE ||
            visited[cx][cy] || !ships[cx][cy])
            continue;

        visited[cx][cy] = true;
        board[cx][cy] = Constants::SUNK;

        stack.push_back(std::make_pair(cx + 1, cy));
        stack.push_back(std::make_pair(cx - 1, cy));
        stack.push_back(std::make_pair(cx, cy + 1));
        stack.push_back(std::make_pair(cx, cy - 1));
    }
}

bool Game::isGameOver() const {
    LockGuard lock(gameMutex);
    return remainingShips1 == 0 || remainingShips2 == 0;
}

Player* Game::getWinner() const {
    LockGuard lock(gameMutex);
    if (remainingShips1 == 0)
        return player2;
    if (remainingShips2 == 0)
        return player1;
    return nullptr;
}

std::string Game::getName() const 
{
    return name;
}

int Game::getId() const 
{
    return gameId;
}

bool Game::getIsActive() const {
    LockGuard lock(gameMutex);
    return isActive;
}

void Game::setInActive() {
    LockGuard lock(gameMutex);
    isActive = false;
}

Player* Game::getCurrentTurn() const {
    LockGuard lock(gameMutex);
    return currentTurn;
}

void Game::setCurrentTurn(Player* player) {
    LockGuard lock(gameMutex);
    currentTurn = player;
}

bool Game::setPlayerReady(Player* player) {
    LockGuard lock(gameMutex);
    if (player == player1) {
        player1Ready = true;
        return true;
    } else if (player == player2) {
        player2Ready = true;
        return true;
    }

    return false;
}

bool Game::areBothReady() const {
    LockGuard lock(gameMutex);
    return player1Ready && player2Ready;
}

bool Game::isValidPlacement(int x, int y, int size, bool horizontal,
                            const std::vector<std::vector<bool>>& board) const {
    if (horizontal) {
        int i = 0;
        int dx = 0;
        int dy = 0;

        if (y + size > Constants::BOARD_SIZE)
            return false;
        for (i = 0; i < size; ++i) {
            if (board[x][y + i])
                return false;
            for (dx = -1; dx <= 1; ++dx) {
                for (dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx;
                    int ny = y + i + dy;
                    if (nx >= 0 && nx < Constants::BOARD_SIZE && ny >= 0 &&
                        ny < Constants::BOARD_SIZE && board[nx][ny])
                        return false;
                }
            }
        }
    } else {
        int i = 0;
        int dx = 0;
        int dy = 0;

        if (x + size > Constants::BOARD_SIZE)
            return false;
        for (i = 0; i < size; ++i) {
            if (board[x + i][y])
                return false;
            for (dx = -1; dx <= 1; ++dx) {
                for (dy = -1; dy <= 1; ++dy) {
                    int nx = x + i + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < Constants::BOARD_SIZE && ny >= 0 &&
                        ny < Constants::BOARD_SIZE && board[nx][ny])
                        return false;
                }
            }
        }
    }

    return false;
}

bool Game::placeShip(Player* player, int x, int y, int size, bool horizontal) {
    LockGuard lock(gameMutex);

    std::vector<std::vector<bool>>& targetShips = (player == player1) ? ships1 : ships2;
    std::vector<std::vector<int>>& targetBoard = (player == player1) ? board1 : board2;

    if (!isValidPlacement(x, y, size, horizontal, targetShips))
        return false;

    if (horizontal) {
        int i = 0;
        for (i = 0; i < size; ++i) {
            targetShips[x][y + i] = true;
            targetBoard[x][y + i] = Constants::SHIP;
        }
    }

    if (player == player1)
        ++remainingShips1;
    else
        ++remainingShips2;

    return true;
}

std::string Game::getBoardState(Player* player, bool showShips) const {
    LockGuard lock(gameMutex);

    const std::vector<std::vector<int>>& ownBoard = (player == player1) ? board1 : board2;
    const std::vector<std::vector<int>>& opponentBoard = (player == player1) ? board2 : board1;
    const std::vector<std::vector<bool>>& ownShips = (player == player1) ? ships1 : ships2;

    std::stringstream ss;

    ss << "Ваше поле: " << std::endl;
    ss << " ";

    int i = 0;
    int j = 0;
    for (i = 0; i < Constants::BOARD_SIZE; ++i) {
        ss << i << " ";
    }

    ss << std::endl;

    for (i = 0; i < Constants::BOARD_SIZE; ++i) {
        ss << i << " ";
        for (j = 0; i < Constants::BOARD_SIZE; ++j) {
            if (showShips && ownShips[i][j] && ownBoard[i][j] == Constants::EMPTY) {
                ss << "S";
            } else {
                switch (ownBoard[i][j]) {
                case Constants::EMPTY:
                    ss << ". ";
                    break;
                case Constants::SHIP:
                    ss << "S ";
                    break;
                case Constants::HIT:
                    ss << "X ";
                    break;
                case Constants::MISS:
                    ss << "O ";
                    break;
                case Constants::SUNK:
                    ss << "# ";
                    break;
                default:
                    ss << "?";
                }
            }
        }
        ss << std::endl;
    }

    std::cout << std::endl;
    ss << "Поле противника: " << std::endl;
    ss << "  ";

    for (i = 0; i < Constants::BOARD_SIZE; i++) {
        ss << i << " ";
    }
    ss << std::endl;

    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            switch (opponentBoard[i][j]) {
            case Constants::EMPTY:
                ss << ". ";
                break;
            case Constants::SHIP:
                ss << ". ";
                break;
            case Constants::HIT:
                ss << "X ";
                break;
            case Constants::MISS:
                ss << "O ";
                break;
            case Constants::SUNK:
                ss << "# ";
                break;
            default:
                ss << "? ";
            }
        }
        ss << std::endl;
    }

    return ss.str();
}

int Game::getRemainingShips(Player* player) const {
    LockGuard lock(gameMutex);
    return (player == player1) ? remainingShips1 : remainingShips2;
}
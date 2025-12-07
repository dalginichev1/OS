#include "Game.h"
#include <sstream>
#include <iostream>
#include <algorithm>

Game::Game(const std::string& name, int id, Player* creator) 
    : name(name), gameId(id), player1(creator), player2(nullptr), 
      isActive(true), currentTurn(nullptr), player1Ready(false), 
      player2Ready(false), remainingShips1(0), remainingShips2(0) {
    
    // Инициализация полей
    board1.resize(Constants::BOARD_SIZE, 
                 std::vector<int>(Constants::BOARD_SIZE, Constants::EMPTY));
    board2.resize(Constants::BOARD_SIZE, 
                 std::vector<int>(Constants::BOARD_SIZE, Constants::EMPTY));
    ships1.resize(Constants::BOARD_SIZE, 
                 std::vector<bool>(Constants::BOARD_SIZE, false));
    ships2.resize(Constants::BOARD_SIZE, 
                 std::vector<bool>(Constants::BOARD_SIZE, false));
}

bool Game::joinGame(Player* player) {
    std::lock_guard<std::mutex> lock(gameMutex);
    if (player2 == nullptr && player != player1) {
        player2 = player;
        return true;
    }
    return false;
}

bool Game::isPlayerInGame(Player* player) const {
    std::lock_guard<std::mutex> lock(gameMutex);
    return player == player1 || player == player2;
}

Player* Game::getOpponent(Player* player) const {
    std::lock_guard<std::mutex> lock(gameMutex);
    if (player == player1) return player2;
    if (player == player2) return player1;
    return nullptr;
}

bool Game::makeMove(Player* player, int x, int y) {
    std::lock_guard<std::mutex> lock(gameMutex);
    
    if (x < 0 || x >= Constants::BOARD_SIZE || 
        y < 0 || y >= Constants::BOARD_SIZE) {
        return false;
    }
    
    Player* opponent = getOpponent(player);
    if (!opponent || currentTurn != player) {
        return false;
    }
    
    std::vector<std::vector<int>>& targetBoard = 
        (player == player1) ? board2 : board1;
    std::vector<std::vector<bool>>& targetShips = 
        (player == player1) ? ships2 : ships1;
    
    if (targetBoard[x][y] != Constants::EMPTY && 
        targetBoard[x][y] != Constants::SHIP) {
        return false; // Уже стреляли сюда
    }
    
    bool hit = targetShips[x][y];
    
    if (hit) {
        targetBoard[x][y] = Constants::HIT;
        if (checkSunk(x, y, targetShips, targetBoard)) {
            markSunkShip(x, y, targetBoard, targetShips);
            if (player == player1) {
                remainingShips2--;
            } else {
                remainingShips1--;
            }
        }
    } else {
        targetBoard[x][y] = Constants::MISS;
    }
    
    // Смена хода
    currentTurn = opponent;
    
    updateBoardStrings();
    return hit;
}

bool Game::checkSunk(int x, int y, const std::vector<std::vector<bool>>& ships,
                    const std::vector<std::vector<int>>& board) const {
    // Находим все клетки корабля
    std::vector<std::pair<int, int>> shipCells;
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> visited(
        Constants::BOARD_SIZE, 
        std::vector<bool>(Constants::BOARD_SIZE, false));
    
    stack.push_back({x, y});
    
    while (!stack.empty()) {
        auto [cx, cy] = stack.back();
        stack.pop_back();
        
        if (cx < 0 || cx >= Constants::BOARD_SIZE || 
            cy < 0 || cy >= Constants::BOARD_SIZE || 
            visited[cx][cy] || !ships[cx][cy]) {
            continue;
        }
        
        visited[cx][cy] = true;
        shipCells.push_back({cx, cy});
        
        // Проверяем соседние клетки
        stack.push_back({cx + 1, cy});
        stack.push_back({cx - 1, cy});
        stack.push_back({cx, cy + 1});
        stack.push_back({cx, cy - 1});
    }
    
    // Проверяем, все ли клетки корабля подбиты
    for (const auto& [cx, cy] : shipCells) {
        if (board[cx][cy] != Constants::HIT) {
            return false;
        }
    }
    
    return true;
}

void Game::markSunkShip(int x, int y, std::vector<std::vector<int>>& board,
                       const std::vector<std::vector<bool>>& ships) const {
    std::vector<std::pair<int, int>> shipCells;
    std::vector<std::pair<int, int>> stack;
    std::vector<std::vector<bool>> visited(
        Constants::BOARD_SIZE, 
        std::vector<bool>(Constants::BOARD_SIZE, false));
    
    stack.push_back({x, y});
    
    while (!stack.empty()) {
        auto [cx, cy] = stack.back();
        stack.pop_back();
        
        if (cx < 0 || cx >= Constants::BOARD_SIZE || 
            cy < 0 || cy >= Constants::BOARD_SIZE || 
            visited[cx][cy] || !ships[cx][cy]) {
            continue;
        }
        
        visited[cx][cy] = true;
        shipCells.push_back({cx, cy});
        board[cx][cy] = Constants::SUNK;
        
        stack.push_back({cx + 1, cy});
        stack.push_back({cx - 1, cy});
        stack.push_back({cx, cy + 1});
        stack.push_back({cx, cy - 1});
    }
}

bool Game::isGameOver() const {
    std::lock_guard<std::mutex> lock(gameMutex);
    return remainingShips1 == 0 || remainingShips2 == 0;
}

Player* Game::getWinner() const {
    std::lock_guard<std::mutex> lock(gameMutex);
    if (remainingShips1 == 0) return player2;
    if (remainingShips2 == 0) return player1;
    return nullptr;
}

std::string Game::getName() const {
    return name;
}

int Game::getId() const {
    return gameId;
}

bool Game::getIsActive() const {
    std::lock_guard<std::mutex> lock(gameMutex);
    return isActive;
}

void Game::setInactive() {
    std::lock_guard<std::mutex> lock(gameMutex);
    isActive = false;
}

Player* Game::getCurrentTurn() const {
    std::lock_guard<std::mutex> lock(gameMutex);
    return currentTurn;
}

void Game::setCurrentTurn(Player* player) {
    std::lock_guard<std::mutex> lock(gameMutex);
    currentTurn = player;
}

bool Game::setPlayerReady(Player* player) {
    std::lock_guard<std::mutex> lock(gameMutex);
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
    std::lock_guard<std::mutex> lock(gameMutex);
    return player1Ready && player2Ready;
}

bool Game::isValidPlacement(int x, int y, int size, bool horizontal,
                           const std::vector<std::vector<bool>>& board) const {
    if (horizontal) {
        if (y + size > Constants::BOARD_SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (board[x][y + i]) return false;
            // Проверяем соседние клетки
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx;
                    int ny = y + i + dy;
                    if (nx >= 0 && nx < Constants::BOARD_SIZE && 
                        ny >= 0 && ny < Constants::BOARD_SIZE && 
                        board[nx][ny]) {
                        return false;
                    }
                }
            }
        }
    } else {
        if (x + size > Constants::BOARD_SIZE) return false;
        for (int i = 0; i < size; i++) {
            if (board[x + i][y]) return false;
            // Проверяем соседние клетки
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + i + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < Constants::BOARD_SIZE && 
                        ny >= 0 && ny < Constants::BOARD_SIZE && 
                        board[nx][ny]) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool Game::placeShip(Player* player, int x, int y, int size, bool horizontal) {
    std::lock_guard<std::mutex> lock(gameMutex);
    
    std::vector<std::vector<bool>>& targetShips = 
        (player == player1) ? ships1 : ships2;
    std::vector<std::vector<int>>& targetBoard = 
        (player == player1) ? board1 : board2;
    
    if (!isValidPlacement(x, y, size, horizontal, targetShips)) {
        return false;
    }
    
    if (horizontal) {
        for (int i = 0; i < size; i++) {
            targetShips[x][y + i] = true;
            targetBoard[x][y + i] = Constants::SHIP;
        }
    } else {
        for (int i = 0; i < size; i++) {
            targetShips[x + i][y] = true;
            targetBoard[x + i][y] = Constants::SHIP;
        }
    }
    
    if (player == player1) {
        remainingShips1++;
    } else {
        remainingShips2++;
    }
    
    return true;
}

std::string Game::getBoardState(Player* player, bool showShips) const {
    std::lock_guard<std::mutex> lock(gameMutex);
    
    const std::vector<std::vector<int>>& ownBoard = 
        (player == player1) ? board1 : board2;
    const std::vector<std::vector<int>>& opponentBoard = 
        (player == player1) ? board2 : board1;
    const std::vector<std::vector<bool>>& ownShips = 
        (player == player1) ? ships1 : ships2;
    
    std::stringstream ss;
    
    // Собственное поле
    ss << "Ваше поле:\n";
    ss << "  ";
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss << i << " ";
    }
    ss << "\n";
    
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            if (showShips && ownShips[i][j] && ownBoard[i][j] == Constants::EMPTY) {
                ss << "S ";
            } else {
                switch (ownBoard[i][j]) {
                    case Constants::EMPTY: ss << ". "; break;
                    case Constants::SHIP: ss << "S "; break;
                    case Constants::HIT: ss << "X "; break;
                    case Constants::MISS: ss << "O "; break;
                    case Constants::SUNK: ss << "# "; break;
                    default: ss << "? ";
                }
            }
        }
        ss << "\n";
    }
    
    ss << "\nПоле противника:\n";
    ss << "  ";
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss << i << " ";
    }
    ss << "\n";
    
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            switch (opponentBoard[i][j]) {
                case Constants::EMPTY: ss << ". "; break;
                case Constants::SHIP: ss << ". "; break; // Не показываем корабли противника
                case Constants::HIT: ss << "X "; break;
                case Constants::MISS: ss << "O "; break;
                case Constants::SUNK: ss << "# "; break;
                default: ss << "? ";
            }
        }
        ss << "\n";
    }
    
    return ss.str();
}

void Game::updateBoardStrings() {
    std::stringstream ss1, ss2;
    
    // Для игрока 1
    ss1 << "Ваше поле:\n";
    ss1 << "  ";
    for (int i = 0; i < Constants::BOARD_SIZE; i++) ss1 << i << " ";
    ss1 << "\n";
    
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss1 << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            switch (board1[i][j]) {
                case Constants::EMPTY: ss1 << ". "; break;
                case Constants::SHIP: ss1 << "S "; break;
                case Constants::HIT: ss1 << "X "; break;
                case Constants::MISS: ss1 << "O "; break;
                case Constants::SUNK: ss1 << "# "; break;
                default: ss1 << "? ";
            }
        }
        ss1 << "\n";
    }
    
    ss1 << "\nПоле противника:\n";
    ss1 << "  ";
    for (int i = 0; i < Constants::BOARD_SIZE; i++) ss1 << i << " ";
    ss1 << "\n";
    
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss1 << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            switch (board2[i][j]) {
                case Constants::EMPTY: ss1 << ". "; break;
                case Constants::SHIP: ss1 << ". "; break;
                case Constants::HIT: ss1 << "X "; break;
                case Constants::MISS: ss1 << "O "; break;
                case Constants::SUNK: ss1 << "# "; break;
                default: ss1 << "? ";
            }
        }
        ss1 << "\n";
    }
    
    player1BoardStr = ss1.str();
    
    // Для игрока 2 (симметрично)
    ss2 << "Ваше поле:\n";
    ss2 << "  ";
    for (int i = 0; i < Constants::BOARD_SIZE; i++) ss2 << i << " ";
    ss2 << "\n";
    
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss2 << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            switch (board2[i][j]) {
                case Constants::EMPTY: ss2 << ". "; break;
                case Constants::SHIP: ss2 << "S "; break;
                case Constants::HIT: ss2 << "X "; break;
                case Constants::MISS: ss2 << "O "; break;
                case Constants::SUNK: ss2 << "# "; break;
                default: ss2 << "? ";
            }
        }
        ss2 << "\n";
    }
    
    ss2 << "\nПоле противника:\n";
    ss2 << "  ";
    for (int i = 0; i < Constants::BOARD_SIZE; i++) ss2 << i << " ";
    ss2 << "\n";
    
    for (int i = 0; i < Constants::BOARD_SIZE; i++) {
        ss2 << i << " ";
        for (int j = 0; j < Constants::BOARD_SIZE; j++) {
            switch (board1[i][j]) {
                case Constants::EMPTY: ss2 << ". "; break;
                case Constants::SHIP: ss2 << ". "; break;
                case Constants::HIT: ss2 << "X "; break;
                case Constants::MISS: ss2 << "O "; break;
                case Constants::SUNK: ss2 << "# "; break;
                default: ss2 << "? ";
            }
        }
        ss2 << "\n";
    }
    
    player2BoardStr = ss2.str();
}

int Game::getRemainingShips(Player* player) const {
    std::lock_guard<std::mutex> lock(gameMutex);
    return (player == player1) ? remainingShips1 : remainingShips2;
}
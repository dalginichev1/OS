#pragma once

#include <string>
#include <vector>
#include "Player.h"
#include "Constants.h"
#include "Mutex.h"

class Game
{
    private:
        std::string name;
        int gameId;
        Player* player1;
        Player* player2;
        bool isActive;
        Player* currentTurn;
        bool player1Ready;
        bool player2Ready;
        mutable Mutex gameMutex;

        std::vector<std::vector<int>> board1;
        std::vector<std::vector<int>> board2;
        std::vector<std::vector<bool>> ships1;
        std::vector<std::vector<bool>> ships2;

        int remainingShips1;
        int remainingShips2;
    
    public:
        Game(const std::string& name, int id, Player* creator);

        bool joinGame(Player* player);
        bool isPlayerInGame(Player* player) const;
        Player * getOpponent(Player * player) const;
        bool makeMove(Player* player, int x, int y);
        bool isGameOver() const;
        Player * getWinner() const;

        std::string getName() const;
        int getId() const;
        void setInActive();
        bool getIsActive() const;
        Player * getCurrentTurn() const;
        void setCurrentTurn(Player * player);

        bool setPlayerReady(Player * player);
        bool areBothReady() const;

        bool placeShip(Player * player, int x, int y, int size, bool horizontal);
        std::string getBoardState(Player * player, bool showShips) const;
        int getRemainingShips(Player * player) const;

    private:
        bool isValidPlacement(int x, int y, int size, bool horizontal, const std::vector<std::vector<bool>>& board) const;
        bool checkSunk(int x, int y, const std::vector<std::vector<bool>>& ships, const std::vector<std::vector<int>>& board) const;
        void markSunkShip(int x, int y, std::vector<std::vector<int>>& board, const std::vector<std::vector<bool>>& ships) const;
};
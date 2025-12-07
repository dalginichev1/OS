#pragma once

#include <string>
#include <vector>
#include "Mutex.h"

class Player
{
    private:
        std::string login;
        int wins;
        int losses;
        int playerId;
        mutable Mutex statsMutex;

    public:
        Player(const std::string& login, int id);

        std::string getLogin() const;
        int getWins() const;
        int getLosses() const;
        int getId() const;

        void addWin();
        void addLoss();

        std::string getStats() const;
};
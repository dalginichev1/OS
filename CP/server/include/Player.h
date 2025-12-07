#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <mutex>

class Player {
private:
    std::string login;
    int wins;
    int losses;
    int playerId;
    mutable std::mutex statsMutex;
    
public:
    Player(const std::string& login, int id);
    ~Player() = default;
    
    std::string getLogin() const;
    int getWins() const;
    int getLosses() const;
    int getId() const;
    
    void addWin();
    void addLoss();
    
    std::string getStats() const;
};

#endif
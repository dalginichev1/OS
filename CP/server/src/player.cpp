#include "Player.h"
#include <sstream>

Player::Player(const std::string& login, int id) 
    : login(login), wins(0), losses(0), playerId(id) {}

std::string Player::getLogin() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    return login;
}

int Player::getWins() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    return wins;
}

int Player::getLosses() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    return losses;
}

int Player::getId() const {
    return playerId;
}

void Player::addWin() {
    std::lock_guard<std::mutex> lock(statsMutex);
    wins++;
}

void Player::addLoss() {
    std::lock_guard<std::mutex> lock(statsMutex);
    losses++;
}

std::string Player::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    std::stringstream ss;
    ss << "Статистика игрока " << login << ":\n";
    ss << "Побед: " << wins << "\n";
    ss << "Поражений: " << losses << "\n";
    ss << "Всего игр: " << (wins + losses) << "\n";
    if (wins + losses > 0) {
        double winRate = (static_cast<double>(wins) / (wins + losses)) * 100;
        ss << "Процент побед: " << winRate << "%\n";
    }
    return ss.str();
}
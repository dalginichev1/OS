#include "Player.h"

#include <sstream>

Player::Player(const std::string& login, int id) : login(login), wins(0), losses(0), playerId(id) {
}

std::string Player::getLogin() const {
    LockGuard lock(statsMutex);
    return login;
}

int Player::getWins() const {
    LockGuard lock(statsMutex);
    return wins;
}

int Player::getLosses() const {
    LockGuard lock(statsMutex);
    return losses;
}

int Player::getId() const {
    return playerId;
}

void Player::addWin() {
    LockGuard lock(statsMutex);
    ++wins;
}

void Player::addLoss() {
    LockGuard lock(statsMutex);
    ++losses;
}

std::string Player::getStats() const {
    LockGuard lock(statsMutex);
    std::stringstream ss;
    ss << "Данные игрока " << login << std::endl;
    ss << "Побед: " << wins << std::endl;
    ss << "Поражений: " << losses << std::endl;
    ss << "Всего игр: " << (wins + losses) << std::endl;
    if (wins + losses > 0) {
        double winRate = (static_cast<double>(wins) / (wins + losses)) * 100;
        ss << "Процент побед: " << winRate << std::endl;
    }

    return ss.str();
}
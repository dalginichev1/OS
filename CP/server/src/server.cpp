#include "Server.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

#include "Protocol.h"

struct ClientThreadData {
    Server* server;
    int clientPipe;
};

Server::Server(const std::string& pipeName)
    : nextPlayerId(1), nextGameId(1), serverPipeFd(-1), running(false), serverPipeName(pipeName) {
    srand(time(NULL));
}

Server::~Server() {
    stop();
}

void Server::run() {
    std::cout << "===Запуск сервера Морской бой ===" << std::endl;

    initializeServerPipe();
    {
        LockGuard lock(serverMutex);
        running = true;
    }

    acceptConnections();
}

void Server::stop() {
    {
        LockGuard lock(serverMutex);
        running = false;
    }

    if (serverPipeFd != -1) {
        close(serverPipeFd);
        serverPipeFd = -1;
    }

    unlink(serverPipeName.c_str());

    std::cout << "Сервер приостановлен" << std::endl;
}

int Server::getNextPlayerId() {
    LockGuard lock(serverMutex);
    return nextPlayerId++;
}

int Server::getNextGameId() {
    LockGuard lock(serverMutex);
    return nextGameId++;
}

bool Server::isRunning() const {
    LockGuard lock(serverMutex);
    return running;
}

void Server::setRunning(bool value) {
    LockGuard lock(serverMutex);
    running = value;
}

void Server::initializeServerPipe() {
    unlink(serverPipeName.c_str());

    if (mkfifo(serverPipeName.c_str(), 0666) == -1) {
        std::cerr << "Ошибка создания pipe: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::cout << "Pipe создан: " << serverPipeName << std::endl;
}

void Server::acceptConnections() {
    std::cout << "Ожидание подключений..." << std::endl;
    
    while (isRunning()) {
        serverPipeFd = open(serverPipeName.c_str(), O_RDONLY | O_NONBLOCK);
        if (serverPipeFd == -1) {
            std::cerr << "Ошибка открытия pipe: " << strerror(errno) << std::endl;
            sleep(1);
            continue;
        }
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverPipeFd, &readfds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ret = select(serverPipeFd + 1, &readfds, NULL, NULL, &timeout);
        
        if (ret > 0 && FD_ISSET(serverPipeFd, &readfds)) {
            char buffer[Constants::BUFFER_SIZE];
            ssize_t bytesRead = read(serverPipeFd, buffer, sizeof(buffer) - 1);
            
            std::cout << "[SERVER DEBUG] Получено " << bytesRead << " байт" << std::endl;
            
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::string message(buffer);
                std::cout << "[SERVER DEBUG] Сообщение: " << message << std::endl;
                
                std::vector<std::string> tokens = split(message, ' ');
                if (!tokens.empty() && tokens[0] == Protocol::LOGIN) {
                    // Создаем структуру для передачи данных в поток
                    ClientThreadData* threadData = new ClientThreadData;
                    threadData->server = this;
                    threadData->clientPipe = -1; // Не используем здесь
                    
                    // Создаем поток для обработки логина
                    pthread_t thread;
                    pthread_create(&thread, NULL, Server::clientThreadFunc, threadData);
                    pthread_detach(thread);
                    
                    // Обрабатываем логин сразу здесь, так как нам нужен pipe клиента
                    processMessage(-1, message, -1); // clientPipe не нужен здесь
                }
            }
        }
        
        close(serverPipeFd);
        usleep(100000);
    }
}

void* Server::clientThreadFunc(void* arg) {
    ClientThreadData* data = static_cast<ClientThreadData*>(arg);
    Server* server = data->server;
    int clientPipe = data->clientPipe;
    delete data;

    server->handleClient(clientPipe);
    return NULL;
}

void Server::handleClient(int clientPipe) {
    int playerId = -1;

    try {
        sleep(1);

        while (isRunning()) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(clientPipe, &readfds);

            struct timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            int ret = select(clientPipe + 1, &readfds, NULL, NULL, &timeout);

            if (ret > 0 && FD_ISSET(clientPipe, &readfds)) {
                char buffer[Constants::BUFFER_SIZE];
                ssize_t bytesRead = read(clientPipe, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0)
                    break;

                buffer[bytesRead] = '\0';
                std::string message(buffer);

                if (playerId == -1 && message.find(Protocol::LOGIN) == 0) {
                    processMessage(clientPipe, message, playerId);
                } else if (ret == 0)
                    continue;
                else
                    break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка в обработке игрока: " << e.what() << std::endl;
    }

    if (playerId != -1) {
        cleanupPlayer(playerId);
    }

    close(clientPipe);
    std::cout << "Игрок отключен" << std::endl;
}

void Server::processMessage(int clientPipe, const std::string& message, int playerId) {
    std::vector<std::string> tokens = split(message, ' ');
    if (tokens.empty())
        return;

    std::string command = tokens[0];

    if (command == Protocol::LOGIN) {
        handleLogin(clientPipe, tokens);
        playerId = findPlayerIdByPipe(clientPipe);
    } else if (command == Protocol::CREATE_GAME) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleCreateGame(clientPipe, tokens, playerId);
    } else if (command == Protocol::JOIN_GAME) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleJoinGame(clientPipe, tokens, playerId);
    } else if (command == Protocol::LIST_GAMES) {
        handleListGames(clientPipe);
    } else if (command == Protocol::GET_STATISTICS) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleGetStats(clientPipe, playerId);
    } else if (command == Protocol::PLACE_SHIP) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handlePlaceShip(clientPipe, tokens, playerId);
    } else if (command == Protocol::MAKE_SHOT) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleMakeShot(clientPipe, tokens, playerId);
    } else if (command == Protocol::GET_BOARD) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleGetBoard(clientPipe, playerId);
    } else if (command == Protocol::READY) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleReady(clientPipe, playerId);
    } else if (command == Protocol::QUIT) {
        if (playerId == -1)
            playerId = findPlayerIdByPipe(clientPipe);
        handleQuit(clientPipe, playerId);
    } else {
        sendResponse(clientPipe, Protocol::ERROR + "Неизвестная команда");
    }
}

void Server::handleLogin(int clientPipe, const std::vector<std::string>& tokens) {
    std::cout << "[SERVER] handleLogin вызван" << std::endl;

    if (tokens.size() < 3) { 
        std::cout << "[SERVER] Ошибка: мало параметров" << std::endl;
        return;
    }

    std::string login = tokens[1];
    std::cout << "[SERVER] Логин: " << login << std::endl;

    std::string clientPipeName = tokens[tokens.size() - 1];
    if (clientPipeName.find("PIPE:") != 0) {
        std::cout << "[SERVER] Ошибка: нет PIPE: в команде" << std::endl;
        return;
    }
    
    clientPipeName = clientPipeName.substr(5); 
    std::cout << "[SERVER] Клиентский pipe: " << clientPipeName << std::endl;

    std::cout << "[SERVER] Открываю pipe..." << std::endl;
    int fd = open(clientPipeName.c_str(), O_WRONLY);
    if (fd == -1) {
        std::cout << "[SERVER] Ошибка open: " << strerror(errno) << std::endl;
        return;
    }
    
    std::cout << "[SERVER] Pipe открыт, пишу ответ..." << std::endl;
    std::string response = "OK Добро пожаловать";
    ssize_t written = write(fd, response.c_str(), response.length());
    std::cout << "[SERVER] Записано " << written << " байт" << std::endl;
    
    close(fd);
    std::cout << "[SERVER] Готово!" << std::endl;
}

void Server::handleCreateGame(int clientPipe, const std::vector<std::string>& tokens,
                              int playerId) {
    if (tokens.size() < 2) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }

    std::string gameName = tokens[1];

    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    for (const auto& gamePair : games) {
        if (gamePair.second->getName() == gameName && gamePair.second->getIsActive()) {
            sendResponse(clientPipe, Protocol::ERROR + " Игра с таким именем уже существует");
            return;
        }
    }

    int gameId = getNextGameId();
    games[gameId] = std::shared_ptr<Game>(new Game(gameName, gameId, playerIt->second.get()));
    playerToGame[playerId] = gameId;

    std::stringstream ss;
    ss << Protocol::GAME_CREATED << " " << gameId << " " << gameName;
    sendResponse(clientPipe, ss.str());

    std::cout << "Игра создана: " << gameName << " (ID: " << gameId << ") игроком "
              << playerIt->second->getLogin() << std::endl;
}

void Server::handleJoinGame(int clientPipe, const std::vector<std::string>& tokens, int playerId) {
    if (tokens.size() < 2) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }

    std::string gameName = tokens[1];

    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    for (auto& gamePair : games) {
        if (gamePair.second->getName() == gameName && gamePair.second->getIsActive()) {
            if (gamePair.second->joinGame(playerIt->second.get())) {
                playerToGame[playerId] = gamePair.first;

                Player* creator = gamePair.second->getOpponent(playerIt->second.get());
                if (creator) {
                    sendToPlayer(creator->getId(),
                                 Protocol::OPPONENT_FOUND + " " + playerIt->second->getLogin());
                }

                sendResponse(clientPipe, Protocol::GAME_JOINED + " " + gameName);

                std::cout << "Игрок " << playerIt->second->getLogin() << " присоединился к игре "
                          << gameName << std::endl;
                return;
            } else {
                sendResponse(clientPipe, Protocol::ERROR + " Не удалось присоединиться к игре");
                return;
            }
        }
    }

    sendResponse(clientPipe, Protocol::ERROR + " Игра не найдена");
}

void Server::handleListGames(int clientPipe) {
    LockGuard lock(serverMutex);

    std::stringstream ss;
    ss << Protocol::GAME_LIST;

    int count = 0;
    for (const auto& gamePair : games) {
        if (gamePair.second->getIsActive()) {
            ss << " " << gamePair.second->getName() << ":1";
            ++count;
        }
    }

    if (count == 0) {
        ss << " Нет доступных игр" << std::endl;
    }

    sendResponse(clientPipe, ss.str());
}

void Server::handleGetStats(int clientPipe, int playerId) {
    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    std::string stats = playerIt->second->getStats();
    sendResponse(clientPipe, Protocol::STATS_RESPONSE + " " + stats);
}

void Server::handlePlaceShip(int clientPipe, const std::vector<std::string>& tokens, int playerId) {
    if (tokens.size() < 5) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }

    int x = atoi(tokens[1].c_str());
    int y = atoi(tokens[2].c_str());
    int size = atoi(tokens[3].c_str());
    bool horizontal = (tokens[4] == "1");

    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    int gameId = findGameIdByPlayer(playerId);
    if (gameId == -1) {
        sendResponse(clientPipe, Protocol::ERROR + " Вы не в игре");
        return;
    }

    auto gameIt = games.find(gameId);
    if (gameIt == games.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игра не найдена");
        return;
    }

    if (gameIt->second->placeShip(playerIt->second.get(), x, y, size, horizontal)) {
        sendResponse(clientPipe, Protocol::OK + " Корабль размещен");
    } else {
        sendResponse(clientPipe, Protocol::ERROR + " Не удалось разместить корабль");
    }
}

void Server::handleMakeShot(int clientPipe, const std::vector<std::string>& tokens, int playerId) {
    if (tokens.size() < 3) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }

    int x = atoi(tokens[1].c_str());
    int y = atoi(tokens[2].c_str());

    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    int gameId = findGameIdByPlayer(playerId);
    if (gameId == -1) {
        sendResponse(clientPipe, Protocol::ERROR + " Вы не в игре");
        return;
    }

    auto gameIt = games.find(gameId);
    if (gameIt == games.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игра не найдена");
        return;
    }

    if (gameIt->second->getCurrentTurn() != playerIt->second.get()) {
        sendResponse(clientPipe, Protocol::ERROR + " Не ваш ход");
        return;
    }

    bool hit = gameIt->second->makeMove(playerIt->second.get(), x, y);

    if (gameIt->second->isGameOver()) {
        Player* winner = gameIt->second->getWinner();
        Player* loser = gameIt->second->getOpponent(winner);

        if (winner) {
            winner->addWin();
            sendToPlayer(winner->getId(), Protocol::WIN);
        }
        if (loser) {
            loser->addLoss();
            sendToPlayer(loser->getId(), Protocol::LOSE);
        }

        cleanupGame(gameId);
    } else {
        std::string result = hit ? Protocol::HIT : Protocol::MISS;
        sendResponse(clientPipe, result);

        Player* opponent = gameIt->second->getOpponent(playerIt->second.get());
        if (opponent) {
            std::stringstream ss;
            ss << Protocol::YOUR_TURN << " " << x << " " << y << " " << (hit ? "1" : "0");
            sendToPlayer(opponent->getId(), ss.str());
        }
    }
}

void Server::handleReady(int clientPipe, int playerId) {
    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    int gameId = findGameIdByPlayer(playerId);
    if (gameId == -1) {
        sendResponse(clientPipe, Protocol::ERROR + " Вы не в игре");
        return;
    }

    auto gameIt = games.find(gameId);
    if (gameIt == games.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игра не найдена");
        return;
    }

    if (gameIt->second->setPlayerReady(playerIt->second.get())) {
        sendResponse(clientPipe, Protocol::OK + " Готов к игре");

        if (gameIt->second->areBothReady()) {
            Player* firstPlayer = (rand() % 2 == 0)
                                      ? gameIt->second->getOpponent(playerIt->second.get())
                                      : playerIt->second.get();
            gameIt->second->setCurrentTurn(firstPlayer);

            if (firstPlayer == playerIt->second.get()) {
                sendResponse(clientPipe, Protocol::YOUR_TURN + " 0 0 0");
            } else {
                sendResponse(clientPipe, Protocol::OPPONENT_TURN);
            }

            Player* opponent = gameIt->second->getOpponent(playerIt->second.get());
            if (opponent) {
                if (firstPlayer == opponent)
                    sendToPlayer(opponent->getId(), Protocol::YOUR_TURN + " 0 0 0");
                else
                    sendToPlayer(opponent->getId(), Protocol::OPPONENT_TURN);
            }
            std::cout << "Игра " << gameIt->second->getName() << " началась" << std::endl;
        }
    } else
        sendResponse(clientPipe, Protocol::ERROR + " Не удалось установить статус готовности");
}

void Server::handleGetBoard(int clientPipe, int playerId) {
    LockGuard lock(serverMutex);

    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }

    int gameId = findGameIdByPlayer(playerId);
    if (gameId == -1) {
        sendResponse(clientPipe, Protocol::ERROR + " Вы не в игре");
        return;
    }

    auto gameIt = games.find(gameId);
    if (gameIt == games.find(gameId)) {
        sendResponse(clientPipe, Protocol::ERROR + " Игра не найдена");
        return;
    }

    std::string boardState = gameIt->second->getBoardState(playerIt->second.get(), true);
    sendResponse(clientPipe, Protocol::BOARD_DATA + " " + boardState);
}

void Server::handleQuit(int clientPipe, int playerId) {
    LockGuard lock(serverMutex);

    cleanupPlayer(playerId);
    sendResponse(clientPipe, Protocol::OK + " До новых встреч");
}

void Server::sendResponse(int clientPipe, const std::string& response) {
    if (clientPipe != -1)
        write(clientPipe, response.c_str(), response.length());
}

bool Server::sendToPlayer(int playerId, const std::string& message) {
    LockGuard lock(serverMutex);
    auto pipeIt = clientPipes.find(playerId);
    if (pipeIt != clientPipes.end() && pipeIt->second != -1) {
        write(pipeIt->second, message.c_str(), message.length());
        return true;
    }
    return false;
}

std::vector<std::string> Server::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if (!token.empty())
            tokens.push_back(token);
    }

    return tokens;
}

void Server::cleanupPlayer(int playerId) {
    LockGuard lock(serverMutex);

    int gameId = findGameIdByPlayer(playerId);
    if (gameId != -1) {
        cleanupGame(gameId);
    }

    auto playerIt = players.find(playerId);
    if (playerIt != players.end()) {
        playerLogins.erase(playerIt->second->getLogin());
        players.erase(playerId);
    }

    clientPipes.erase(playerId);
    playerToGame.erase(playerId);
}

void Server::cleanupGame(int gameId) {
    LockGuard lock(serverMutex);

    auto gameIt = games.find(gameId);
    if (gameIt != games.end()) {
        for (auto it = playerToGame.begin(); it != playerToGame.end();) {
            if (it->second == gameId)
                it = playerToGame.erase(it);
            else
                ++it;
        }

        games.erase(gameId);
        std::cout << "Игра ID: " << gameId << " завершена и удалена" << std::endl;
    }
}

int Server::findPlayerIdByPipe(int clientPipe) const {
    LockGuard lock(serverMutex);

    for (const auto& pair : clientPipes) {
        if (pair.second == clientPipe)
            return pair.first;
    }

    return -1;
}

int Server::findGameIdByPlayer(int playerId) const {
    LockGuard lock(serverMutex);

    auto it = playerToGame.find(playerId);
    if (it != playerToGame.end())
        return it->second;

    return -1;
}
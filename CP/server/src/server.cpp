#include "Server.h"
#include "Protocol.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>

Server::Server(const std::string& pipeName) 
    : serverPipeName(pipeName), nextPlayerId(1), nextGameId(1), 
      serverPipeFd(-1), running(false) {}

Server::~Server() {
    stop();
}

void Server::run() {
    std::cout << "=== Запуск сервера Морской бой ===" << std::endl;
    std::cout << "Используется именованный pipe: " << serverPipeName << std::endl;
    
    initializeServerPipe();
    running = true;
    
    acceptConnections();
}

void Server::stop() {
    running = false;
    
    if (serverPipeFd != -1) {
        close(serverPipeFd);
        serverPipeFd = -1;
    }
    
    // Удаляем именованный pipe
    unlink(serverPipeName.c_str());
    
    std::cout << "Сервер остановлен" << std::endl;
}

void Server::initializeServerPipe() {
    // Удаляем старый pipe если существует
    unlink(serverPipeName.c_str());
    
    // Создаем новый именованный pipe
    if (mkfifo(serverPipeName.c_str(), 0666) == -1) {
        std::cerr << "Ошибка создания pipe: " << strerror(errno) << std::endl;
        exit(1);
    }
    
    std::cout << "Pipe создан: " << serverPipeName << std::endl;
}

void Server::acceptConnections() {
    std::cout << "Ожидание подключений..." << std::endl;
    
    while (running) {
        // Открываем pipe для чтения
        serverPipeFd = open(serverPipeName.c_str(), O_RDONLY | O_NONBLOCK);
        if (serverPipeFd == -1) {
            std::cerr << "Ошибка открытия pipe: " << strerror(errno) << std::endl;
            sleep(1);
            continue;
        }
        
        // Используем poll для ожидания данных
        struct pollfd fds[1];
        fds[0].fd = serverPipeFd;
        fds[0].events = POLLIN;
        
        int ret = poll(fds, 1, 1000); // Таймаут 1 секунда
        
        if (ret > 0 && (fds[0].revents & POLLIN)) {
            // Клиент пытается подключиться
            char buffer[Constants::BUFFER_SIZE];
            ssize_t bytesRead = read(serverPipeFd, buffer, sizeof(buffer) - 1);
            
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::string message(buffer);
                
                // Первое сообщение должно быть LOGIN
                std::vector<std::string> tokens = split(message, ' ');
                if (!tokens.empty() && tokens[0] == Protocol::LOGIN) {
                    // Открываем отдельный pipe для клиента
                    std::string clientPipeName = serverPipeName + "_client_" + 
                                                std::to_string(nextPlayerId);
                    
                    // Создаем pipe для ответов клиенту
                    if (mkfifo(clientPipeName.c_str(), 0666) == -1) {
                        std::cerr << "Ошибка создания клиентского pipe: " 
                                 << strerror(errno) << std::endl;
                        continue;
                    }
                    
                    // Открываем pipe для записи
                    int clientWriteFd = open(clientPipeName.c_str(), O_WRONLY);
                    if (clientWriteFd == -1) {
                        std::cerr << "Ошибка открытия клиентского pipe: " 
                                 << strerror(errno) << std::endl;
                        unlink(clientPipeName.c_str());
                        continue;
                    }
                    
                    // Создаем поток для обработки клиента
                    std::thread clientThread(&Server::handleClient, this, clientWriteFd);
                    clientThread.detach();
                    
                    // Отправляем клиенту имя его pipe
                    std::string response = Protocol::OK + " " + clientPipeName;
                    write(clientWriteFd, response.c_str(), response.length());
                    close(clientWriteFd);
                }
            }
        }
        
        close(serverPipeFd);
        
        // Небольшая пауза чтобы не грузить CPU
        usleep(100000);
    }
}

void Server::handleClient(int clientPipe) {
    int playerId = -1;
    std::string clientPipeName = "";
    
    try {
        // Читаем имя pipe для обмена данными
        char buffer[Constants::BUFFER_SIZE];
        
        // Ждем пока клиент откроет pipe для чтения
        sleep(1);
        
        // Теперь можем читать команды от клиента
        while (running) {
            struct pollfd fds[1];
            fds[0].fd = clientPipe;
            fds[0].events = POLLIN;
            
            int ret = poll(fds, 1, 1000);
            
            if (ret > 0 && (fds[0].revents & POLLIN)) {
                ssize_t bytesRead = read(clientPipe, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0) {
                    break; // Клиент отключился
                }
                
                buffer[bytesRead] = '\0';
                std::string message(buffer);
                
                // Обрабатываем сообщение
                if (playerId == -1 && message.find(Protocol::LOGIN) == 0) {
                    processMessage(clientPipe, message, playerId);
                    // После LOGIN у нас должен быть playerId
                } else if (playerId != -1) {
                    processMessage(clientPipe, message, playerId);
                }
            } else if (ret == 0) {
                // Таймаут, продолжаем ждать
                continue;
            } else {
                // Ошибка
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка в обработке клиента: " << e.what() << std::endl;
    }
    
    // Очистка
    if (playerId != -1) {
        cleanupPlayer(playerId);
    }
    
    close(clientPipe);
    if (!clientPipeName.empty()) {
        unlink(clientPipeName.c_str());
    }
    
    std::cout << "Клиент отключен" << std::endl;
}

void Server::processMessage(int clientPipe, const std::string& message, int playerId) {
    std::vector<std::string> tokens = split(message, ' ');
    if (tokens.empty()) return;
    
    std::string command = tokens[0];
    
    if (command == Protocol::LOGIN) {
        handleLogin(clientPipe, tokens);
        // Ищем playerId для этого pipe
        playerId = findPlayerIdByPipe(clientPipe);
    } else if (command == Protocol::CREATE_GAME) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleCreateGame(clientPipe, tokens, playerId);
    } else if (command == Protocol::JOIN_GAME) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleJoinGame(clientPipe, tokens, playerId);
    } else if (command == Protocol::LIST_GAMES) {
        handleListGames(clientPipe);
    } else if (command == Protocol::GET_STATISTICS) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleGetStats(clientPipe, playerId);
    } else if (command == Protocol::PLACE_SHIP) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handlePlaceShip(clientPipe, tokens, playerId);
    } else if (command == Protocol::MAKE_SHOT) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleMakeShot(clientPipe, tokens, playerId);
    } else if (command == Protocol::READY) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleReady(clientPipe, playerId);
    } else if (command == Protocol::GET_BOARD) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleGetBoard(clientPipe, playerId);
    } else if (command == Protocol::QUIT) {
        if (playerId == -1) playerId = findPlayerIdByPipe(clientPipe);
        handleQuit(clientPipe, playerId);
    } else {
        sendResponse(clientPipe, Protocol::ERROR + " Неизвестная команда");
    }
}

void Server::handleLogin(int clientPipe, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }
    
    std::string login = tokens[1];
    
    std::lock_guard<std::mutex> lock(serverMutex);
    
    // Проверяем, существует ли уже игрок с таким логином
    auto it = playerLogins.find(login);
    if (it != playerLogins.end()) {
        // Игрок уже существует
        int existingPlayerId = it->second;
        clientPipes[existingPlayerId] = clientPipe;
        sendResponse(clientPipe, Protocol::OK + " Добро пожаловать обратно, " + login);
    } else {
        // Новый игрок
        int newPlayerId = nextPlayerId++;
        players[newPlayerId] = std::make_shared<Player>(login, newPlayerId);
        playerLogins[login] = newPlayerId;
        clientPipes[newPlayerId] = clientPipe;
        sendResponse(clientPipe, Protocol::OK + " Регистрация успешна, " + login);
    }
}

void Server::handleCreateGame(int clientPipe, const std::vector<std::string>& tokens, int playerId) {
    if (tokens.size() < 2) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }
    
    std::string gameName = tokens[1];
    
    std::lock_guard<std::mutex> lock(serverMutex);
    
    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }
    
    // Проверяем, нет ли уже игры с таким именем
    for (const auto& gamePair : games) {
        if (gamePair.second->getName() == gameName && gamePair.second->getIsActive()) {
            sendResponse(clientPipe, Protocol::ERROR + " Игра с таким именем уже существует");
            return;
        }
    }
    
    int gameId = nextGameId++;
    games[gameId] = std::make_shared<Game>(gameName, gameId, playerIt->second.get());
    playerToGame[playerId] = gameId;
    
    std::stringstream ss;
    ss << Protocol::GAME_CREATED << " " << gameId << " " << gameName;
    sendResponse(clientPipe, ss.str());
    
    std::cout << "Игра создана: " << gameName << " (ID: " << gameId 
              << ") игроком " << playerIt->second->getLogin() << std::endl;
}

void Server::handleJoinGame(int clientPipe, const std::vector<std::string>& tokens, int playerId) {
    if (tokens.size() < 2) {
        sendResponse(clientPipe, Protocol::ERROR + " Неверный формат команды");
        return;
    }
    
    std::string gameName = tokens[1];
    
    std::lock_guard<std::mutex> lock(serverMutex);
    
    auto playerIt = players.find(playerId);
    if (playerIt == players.end()) {
        sendResponse(clientPipe, Protocol::ERROR + " Игрок не найден");
        return;
    }
    
    // Ищем игру
    for (auto& gamePair : games) {
        if (gamePair.second->getName() == gameName && gamePair.second->getIsActive()) {
            if (gamePair.second->joinGame(playerIt->second.get())) {
                playerToGame[playerId] = gamePair.first;
                
                // Отправляем сообщение создателю игры
                Player* creator = gamePair.second->getOpponent(playerIt->second.get());
                if (creator) {
                    sendToPlayer(creator->getId(), 
                                Protocol::OPPONENT_FOUND + " " + playerIt->second->getLogin());
                }
                
                sendResponse(clientPipe, Protocol::GAME_JOINED + " " + gameName);
                
                std::cout << "Игрок " << playerIt->second->getLogin() 
                          << " присоединился к игре " << gameName << std::endl;
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
    std::lock_guard<std::mutex> lock(serverMutex);
    
    std::stringstream ss;
    ss << Protocol::GAME_LIST;
    
    int count = 0;
    for (const auto& gamePair : games) {
        if (gamePair.second->getIsActive()) {
            Player* player1 = nullptr;
            // Получаем первого игрока (создателя)
            // В реальной реализации нужно добавить метод для этого
            
            ss << " " << gamePair.second->getName() << ":1"; // :1 означает ожидает игрока
            count++;
        }
    }
    
    if (count == 0) {
        ss << " Нет доступных игр";
    }
    
    sendResponse(clientPipe, ss.str());
}

void Server::handleGetStats(int clientPipe, int playerId) {
    std::lock_guard<std::mutex> lock(serverMutex);
    
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
    
    int x = std::stoi(tokens[1]);
    int y = std::stoi(tokens[2]);
    int size = std::stoi(tokens[3]);
    bool horizontal = (tokens[4] == "1");
    
    std::lock_guard<std::mutex> lock(serverMutex);
    
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
    
    int x = std::stoi(tokens[1]);
    int y = std::stoi(tokens[2]);
    
    std::lock_guard<std::mutex> lock(serverMutex);
    
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
    
    // Проверяем, ход ли игрока
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
        
        // Очищаем игру
        cleanupGame(gameId);
    } else {
        std::string result = hit ? Protocol::HIT : Protocol::MISS;
        sendResponse(clientPipe, result);
        
        // Уведомляем противника
        Player* opponent = gameIt->second->getOpponent(playerIt->second.get());
        if (opponent) {
            std::stringstream ss;
            ss << Protocol::YOUR_TURN << " " << x << " " << y << " " << (hit ? "1" : "0");
            sendToPlayer(opponent->getId(), ss.str());
        }
    }
}

void Server::handleReady(int clientPipe, int playerId) {
    std::lock_guard<std::mutex> lock(serverMutex);
    
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
        
        // Если оба игрока готовы, начинаем игру
        if (gameIt->second->areBothReady()) {
            // Выбираем случайно, кто ходит первым
            Player* firstPlayer = (rand() % 2 == 0) ? 
                gameIt->second->getOpponent(playerIt->second.get()) : playerIt->second.get();
            gameIt->second->setCurrentTurn(firstPlayer);
            
            // Уведомляем игроков
            if (firstPlayer == playerIt->second.get()) {
                sendResponse(clientPipe, Protocol::YOUR_TURN + " 0 0 0");
            } else {
                sendResponse(clientPipe, Protocol::OPPONENT_TURN);
            }
            
            Player* opponent = gameIt->second->getOpponent(playerIt->second.get());
            if (opponent) {
                if (firstPlayer == opponent) {
                    sendToPlayer(opponent->getId(), Protocol::YOUR_TURN + " 0 0 0");
                } else {
                    sendToPlayer(opponent->getId(), Protocol::OPPONENT_TURN);
                }
            }
            
            std::cout << "Игра " << gameIt->second->getName() << " началась" << std::endl;
        }
    } else {
        sendResponse(clientPipe, Protocol::ERROR + " Не удалось установить статус готовности");
    }
}

void Server::handleGetBoard(int clientPipe, int playerId) {
    std::lock_guard<std::mutex> lock(serverMutex);
    
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
    
    std::string boardState = gameIt->second->getBoardState(playerIt->second.get(), true);
    sendResponse(clientPipe, Protocol::BOARD_DATA + " " + boardState);
}

void Server::handleQuit(int clientPipe, int playerId) {
    std::lock_guard<std::mutex> lock(serverMutex);
    
    cleanupPlayer(playerId);
    sendResponse(clientPipe, Protocol::OK + " До свидания");
}

void Server::sendResponse(int clientPipe, const std::string& response) {
    if (clientPipe != -1) {
        write(clientPipe, response.c_str(), response.length());
    }
}

bool Server::sendToPlayer(int playerId, const std::string& message) {
    std::lock_guard<std::mutex> lock(serverMutex);
    
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
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

void Server::cleanupPlayer(int playerId) {
    std::lock_guard<std::mutex> lock(serverMutex);
    
    // Удаляем игрока из игры
    int gameId = findGameIdByPlayer(playerId);
    if (gameId != -1) {
        cleanupGame(gameId);
    }
    
    // Удаляем из мап
    auto playerIt = players.find(playerId);
    if (playerIt != players.end()) {
        playerLogins.erase(playerIt->second->getLogin());
        players.erase(playerId);
    }
    
    clientPipes.erase(playerId);
    playerToGame.erase(playerId);
}

void Server::cleanupGame(int gameId) {
    auto gameIt = games.find(gameId);
    if (gameIt != games.end()) {
        // Удаляем игроков из маппинга игрок->игра
        for (auto it = playerToGame.begin(); it != playerToGame.end();) {
            if (it->second == gameId) {
                it = playerToGame.erase(it);
            } else {
                ++it;
            }
        }
        
        games.erase(gameId);
        std::cout << "Игра ID: " << gameId << " завершена и удалена" << std::endl;
    }
}

int Server::findPlayerIdByPipe(int clientPipe) const {
    std::lock_guard<std::mutex> lock(serverMutex);
    
    for (const auto& pair : clientPipes) {
        if (pair.second == clientPipe) {
            return pair.first;
        }
    }
    return -1;
}

int Server::findGameIdByPlayer(int playerId) const {
    std::lock_guard<std::mutex> lock(serverMutex);
    
    auto it = playerToGame.find(playerId);
    if (it != playerToGame.end()) {
        return it->second;
    }
    return -1;
}
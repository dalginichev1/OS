#include "Client.h"
#include "../common/Protocol.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>

Client::Client(const std::string& serverPipe) 
    : pipeClient(serverPipe), loggedIn(false), inGame(false), 
      myTurn(false), gameOver(false) {}

void Client::run() {
    clearScreen();
    std::cout << "=== Клиент Морской бой ===" << std::endl;
    
    // Подключаемся к серверу
    if (!pipeClient.connectToServer()) {
        std::cout << "Не удалось подключиться к серверу" << std::endl;
        return;
    }
    
    // Основной цикл
    while (true) {
        if (!loggedIn) {
            handleLogin();
        } else if (!inGame) {
            showMenu();
        } else {
            handleGameLoop();
        }
    }
}

void Client::showMenu() {
    clearScreen();
    std::cout << "=== Меню ===" << std::endl;
    std::cout << "Игрок: " << playerName << std::endl;
    std::cout << "1. Создать игру" << std::endl;
    std::cout << "2. Присоединиться к игре" << std::endl;
    std::cout << "3. Список игр" << std::endl;
    std::cout << "4. Статистика" << std::endl;
    std::cout << "5. Выйти" << std::endl;
    std::cout << "Выберите действие: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore();
    
    switch (choice) {
        case 1:
            handleCreateGame();
            break;
        case 2:
            handleJoinGame();
            break;
        case 3:
            handleListGames();
            break;
        case 4:
            handleGetStats();
            break;
        case 5:
            sendCommand(Protocol::QUIT);
            std::cout << "До свидания!" << std::endl;
            exit(0);
        default:
            std::cout << "Неверный выбор" << std::endl;
            waitForEnter();
    }
}

void Client::handleLogin() {
    clearScreen();
    std::cout << "=== Вход в игру ===" << std::endl;
    
    std::cout << "Введите ваш логин: ";
    std::string login;
    std::getline(std::cin, login);
    
    std::string command = Protocol::LOGIN + " " + login;
    if (sendCommand(command)) {
        std::string response = receiveResponse();
        if (response.find(Protocol::OK) == 0) {
            playerName = login;
            loggedIn = true;
            std::cout << "Успешный вход!" << std::endl;
        } else {
            std::cout << "Ошибка: " << response << std::endl;
        }
    } else {
        std::cout << "Ошибка отправки команды" << std::endl;
    }
    
    waitForEnter();
}

void Client::handleCreateGame() {
    clearScreen();
    std::cout << "=== Создание игры ===" << std::endl;
    
    std::cout << "Введите название игры: ";
    std::string gameName;
    std::getline(std::cin, gameName);
    
    std::string command = Protocol::CREATE_GAME + " " + gameName;
    if (sendCommand(command)) {
        std::string response = receiveResponse();
        if (response.find(Protocol::GAME_CREATED) == 0) {
            inGame = true;
            std::cout << "Игра создана! Ожидаем второго игрока..." << std::endl;
            
            // Ждем подключения второго игрока
            while (inGame && !gameOver) {
                std::string msg = receiveResponse();
                if (!msg.empty()) {
                    if (msg.find(Protocol::OPPONENT_FOUND) == 0) {
                        std::cout << "Противник найден! Начинаем расстановку кораблей." << std::endl;
                        waitForEnter();
                        handlePlaceShips();
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        } else {
            std::cout << "Ошибка: " << response << std::endl;
            waitForEnter();
        }
    }
}

void Client::handleJoinGame() {
    clearScreen();
    std::cout << "=== Присоединение к игре ===" << std::endl;
    
    std::cout << "Введите название игры: ";
    std::string gameName;
    std::getline(std::cin, gameName);
    
    std::string command = Protocol::JOIN_GAME + " " + gameName;
    if (sendCommand(command)) {
        std::string response = receiveResponse();
        if (response.find(Protocol::GAME_JOINED) == 0) {
            inGame = true;
            std::cout << "Вы присоединились к игре!" << std::endl;
            std::cout << "Начинаем расстановку кораблей." << std::endl;
            waitForEnter();
            handlePlaceShips();
        } else {
            std::cout << "Ошибка: " << response << std::endl;
            waitForEnter();
        }
    }
}

void Client::handleListGames() {
    clearScreen();
    std::cout << "=== Список игр ===" << std::endl;
    
    std::string command = Protocol::LIST_GAMES;
    if (sendCommand(command)) {
        std::string response = receiveResponse();
        if (response.find(Protocol::GAME_LIST) == 0) {
            std::string gamesList = response.substr(Protocol::GAME_LIST.length() + 1);
            if (gamesList.empty() || gamesList == "Нет доступных игр") {
                std::cout << "Нет доступных игр" << std::endl;
            } else {
                std::istringstream iss(gamesList);
                std::string game;
                int count = 1;
                while (iss >> game) {
                    size_t pos = game.find(':');
                    if (pos != std::string::npos) {
                        std::string name = game.substr(0, pos);
                        std::string status = game.substr(pos + 1);
                        std::cout << count << ". " << name << " (игроков: " << status << "/2)" << std::endl;
                    } else {
                        std::cout << count << ". " << game << std::endl;
                    }
                    count++;
                }
            }
        } else {
            std::cout << "Ошибка: " << response << std::endl;
        }
    }
    
    waitForEnter();
}

void Client::handleGetStats() {
    clearScreen();
    std::cout << "=== Статистика ===" << std::endl;
    
    std::string command = Protocol::GET_STATISTICS;
    if (sendCommand(command)) {
        std::string response = receiveResponse();
        if (response.find(Protocol::STATS_RESPONSE) == 0) {
            std::string stats = response.substr(Protocol::STATS_RESPONSE.length() + 1);
            std::cout << stats << std::endl;
        } else {
            std::cout << "Ошибка: " << response << std::endl;
        }
    }
    
    waitForEnter();
}

void Client::handlePlaceShips() {
    clearScreen();
    std::cout << "=== Расстановка кораблей ===" << std::endl;
    std::cout << "Разместите ваши корабли на поле 10x10" << std::endl;
    
    playerBoard.initialize();
    
    // Размещаем корабли согласно правилам
    for (int type = 0; type < 5; type++) {
        int size = Constants::SHIP_TYPES[type][0];
        int count = Constants::SHIP_TYPES[type][1];
        
        for (int shipNum = 0; shipNum < count; shipNum++) {
            bool placed = false;
            while (!placed) {
                clearScreen();
                std::cout << "=== Расстановка кораблей ===" << std::endl;
                std::cout << "Корабль размером " << size << " палуб" << std::endl;
                std::cout << "Осталось разместить: " << (count - shipNum) << " таких кораблей" << std::endl;
                std::cout << std::endl;
                std::cout << playerBoard.toString(true) << std::endl;
                
                std::cout << "Введите координаты (x y) и ориентацию (0 - вертикально, 1 - горизонтально): ";
                int x, y, orientation;
                std::cin >> x >> y >> orientation;
                std::cin.ignore();
                
                if (playerBoard.placeShip(x, y, size, orientation == 1)) {
                    placed = true;
                    // Отправляем информацию о корабле на сервер
                    std::string command = Protocol::PLACE_SHIP + " " + 
                                         std::to_string(x) + " " + 
                                         std::to_string(y) + " " + 
                                         std::to_string(size) + " " + 
                                         std::to_string(orientation);
                    sendCommand(command);
                } else {
                    std::cout << "Невозможно разместить корабль здесь. Попробуйте еще раз." << std::endl;
                    waitForEnter();
                }
            }
        }
    }
    
    // Сообщаем серверу, что готовы
    std::cout << "Все корабли размещены!" << std::endl;
    sendCommand(Protocol::READY);
    
    std::cout << "Ожидаем готовности противника..." << std::endl;
    
    // Ждем начала игры
    while (inGame && !gameOver) {
        std::string response = receiveResponse();
        if (!response.empty()) {
            if (response.find(Protocol::YOUR_TURN) == 0) {
                myTurn = true;
                std::cout << "Игра началась! Ваш ход." << std::endl;
                waitForEnter();
                break;
            } else if (response.find(Protocol::OPPONENT_TURN) == 0) {
                myTurn = false;
                std::cout << "Игра началась! Ход противника." << std::endl;
                waitForEnter();
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Client::handleGameLoop() {
    while (inGame && !gameOver) {
        clearScreen();
        
        // Получаем текущее состояние доски
        sendCommand(Protocol::GET_BOARD);
        std::string boardResponse = receiveResponse();
        if (boardResponse.find(Protocol::BOARD_DATA) == 0) {
            std::string boardState = boardResponse.substr(Protocol::BOARD_DATA.length() + 1);
            std::cout << boardState << std::endl;
        }
        
        if (myTurn) {
            handleMakeShot();
        } else {
            std::cout << "Ожидаем ход противника..." << std::endl;
            // Ждем уведомления от сервера
            std::string response = receiveResponse();
            if (!response.empty()) {
                if (response.find(Protocol::YOUR_TURN) == 0) {
                    myTurn = true;
                    // Извлекаем информацию о выстреле противника
                    std::istringstream iss(response);
                    std::string cmd;
                    int x, y, hit;
                    iss >> cmd >> x >> y >> hit;
                    
                    if (hit) {
                        std::cout << "Противник попал в (" << x << ", " << y << ")" << std::endl;
                    } else {
                        std::cout << "Противник промахнулся в (" << x << ", " << y << ")" << std::endl;
                    }
                    waitForEnter();
                } else if (response.find(Protocol::WIN) == 0) {
                    std::cout << "ПОЗДРАВЛЯЕМ! ВЫ ВЫИГРАЛИ!" << std::endl;
                    gameOver = true;
                    inGame = false;
                    waitForEnter();
                    break;
                } else if (response.find(Protocol::LOSE) == 0) {
                    std::cout << "Вы проиграли. Попробуйте еще раз!" << std::endl;
                    gameOver = true;
                    inGame = false;
                    waitForEnter();
                    break;
                }
            }
        }
    }
    
    // После окончания игры возвращаемся в меню
    if (gameOver) {
        gameOver = false;
        playerBoard.initialize();
        opponentBoard.initialize();
    }
}

void Client::handleMakeShot() {
    std::cout << "Ваш ход!" << std::endl;
    std::cout << "Введите координаты для выстрела (x y): ";
    
    int x, y;
    std::cin >> x >> y;
    std::cin.ignore();
    
    std::string command = Protocol::MAKE_SHOT + " " + 
                         std::to_string(x) + " " + 
                         std::to_string(y);
    
    if (sendCommand(command)) {
        std::string response = receiveResponse();
        if (response.find(Protocol::HIT) == 0) {
            std::cout << "ПОПАДАНИЕ!" << std::endl;
            myTurn = true; // При попадании ход остается
        } else if (response.find(Protocol::MISS) == 0) {
            std::cout << "ПРОМАХ!" << std::endl;
            myTurn = false; // При промахе ход переходит
        } else if (response.find(Protocol::WIN) == 0) {
            std::cout << "ПОЗДРАВЛЯЕМ! ВЫ ВЫИГРАЛИ!" << std::endl;
            gameOver = true;
            inGame = false;
        } else if (response.find(Protocol::LOSE) == 0) {
            std::cout << "Вы проиграли. Попробуйте еще раз!" << std::endl;
            gameOver = true;
            inGame = false;
        } else {
            std::cout << "Ошибка: " << response << std::endl;
        }
    }
    
    waitForEnter();
}

bool Client::sendCommand(const std::string& command) {
    return pipeClient.sendMessage(command);
}

std::string Client::receiveResponse() {
    return pipeClient.receiveMessage();
}

void Client::clearScreen() {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape codes для очистки экрана
}

void Client::waitForEnter() {
    std::cout << "Нажмите Enter для продолжения...";
    std::cin.ignore();
}
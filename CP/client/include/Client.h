#pragma once

#include <string>

#include "GameBoard.h"
#include "PipeClient.h"

class Client {
  private:
    PipeClient pipeClient;
    GameBoard playerBoard;
    GameBoard opponentBoard;
    std::string playerName;
    bool loggedIn;
    bool inGame;
    bool myTurn;
    bool gameOver;

    void showMenu();
    void handleLogin();
    void handleCreateGame();
    void handleJoinGame();
    void handleListGames();
    void handleGetStats();
    void handlePlaceShips();
    void handleGameLoop();
    void handleMakeShot();

    bool sendCommand(const std::string& command);
    std::string receiveResponse();

    void clearScreen();
    void waitForEnter();

  public:
    Client(const std::string& serverPipe);

    void run();
};
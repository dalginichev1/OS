#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "PipeClient.h"
#include "GameBoard.h"

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
    
public:
    Client(const std::string& serverPipe);
    ~Client() = default;
    
    void run();
    
private:
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
};

#endif
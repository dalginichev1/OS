#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include "Game.h"
#include "Player.h"
#include "Constants.h"

class Server {
private:
    std::map<int, std::shared_ptr<Player>> players;
    std::map<int, std::shared_ptr<Game>> games;
    std::map<std::string, int> playerLogins;
    std::map<int, int> playerToGame; // playerId -> gameId
    std::map<int, int> clientPipes; // playerId -> pipe fd
    
    mutable std::mutex serverMutex;
    std::atomic<int> nextPlayerId;
    std::atomic<int> nextGameId;
    
    std::string serverPipeName;
    int serverPipeFd;
    std::atomic<bool> running;
    
public:
    Server(const std::string& pipeName);
    ~Server();
    
    void run();
    void stop();
    
private:
    void initializeServerPipe();
    void acceptConnections();
    void handleClient(int clientPipe);
    void processMessage(int clientPipe, const std::string& message, int playerId);
    
    // Обработчики команд
    void handleLogin(int clientPipe, const std::vector<std::string>& tokens);
    void handleCreateGame(int clientPipe, const std::vector<std::string>& tokens, int playerId);
    void handleJoinGame(int clientPipe, const std::vector<std::string>& tokens, int playerId);
    void handleListGames(int clientPipe);
    void handleGetStats(int clientPipe, int playerId);
    void handlePlaceShip(int clientPipe, const std::vector<std::string>& tokens, int playerId);
    void handleMakeShot(int clientPipe, const std::vector<std::string>& tokens, int playerId);
    void handleReady(int clientPipe, int playerId);
    void handleGetBoard(int clientPipe, int playerId);
    void handleQuit(int clientPipe, int playerId);
    
    void sendResponse(int clientPipe, const std::string& response);
    bool sendToPlayer(int playerId, const std::string& message);
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    void cleanupPlayer(int playerId);
    void cleanupGame(int gameId);
    
    int findPlayerIdByPipe(int clientPipe) const;
    int findGameIdByPlayer(int playerId) const;
};

#endif
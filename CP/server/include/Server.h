#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Game.h"
#include "Player.h"
#include "Constants.h"
#include "Mutex.h"

struct ClientThreadData;

class Server
{
    private:
        int nextPlayerId;
        int nextGameId;
        int serverPipeFd;
        bool running;

        std::string serverPipeName;

        std::map<int, std::shared_ptr<Player>> players;
        std::map<int, std::shared_ptr<Game>> games;
        std::map<std::string, int> playerLogins;
        std::map<int, int> playerToGame;
        std::map<int, int> clientPipes;

        mutable Mutex serverMutex;

        void initializeServerPipe();
        void acceptConnections();
        void handleClient(int clientPipe);
        void processMessage(int clientPipe, const std::string& message, int playerId);

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

        static void * clientThreadFunc(void * arg);

        int getNextPlayerId();
        int getNextGameId();
        bool isRunning() const;
        void setRunning(bool value);

    public:
        Server(const std::string& pipeName);
        ~Server();

        void run();
        void stop();
};
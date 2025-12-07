#ifndef PIPECLIENT_H
#define PIPECLIENT_H

#include <string>

class PipeClient {
private:
    std::string serverPipeName;
    std::string clientPipeName;
    int serverPipeFd;
    int clientPipeFd;
    
public:
    PipeClient(const std::string& serverPipe);
    ~PipeClient();
    
    bool connectToServer();
    void disconnect();
    bool sendMessage(const std::string& message);
    std::string receiveMessage(int timeoutMs = 1000);
    
    std::string getClientPipeName() const { return clientPipeName; }
    
private:
    bool createClientPipe();
};

#endif
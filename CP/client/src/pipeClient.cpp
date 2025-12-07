#include "PipeClient.h"
#include "../common/Constants.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>

PipeClient::PipeClient(const std::string& serverPipe) 
    : serverPipeName(serverPipe), clientPipeName(""),
      serverPipeFd(-1), clientPipeFd(-1) {}

PipeClient::~PipeClient() {
    disconnect();
}

bool PipeClient::connectToServer() {
    // Создаем pipe для получения ответов от сервера
    if (!createClientPipe()) {
        return false;
    }
    
    // Открываем серверный pipe для записи
    serverPipeFd = open(serverPipeName.c_str(), O_WRONLY);
    if (serverPipeFd == -1) {
        std::cerr << "Не удалось подключиться к серверу" << std::endl;
        return false;
    }
    
    return true;
}

bool PipeClient::createClientPipe() {
    // Генерируем уникальное имя для клиентского pipe
    pid_t pid = getpid();
    clientPipeName = serverPipeName + "_client_" + std::to_string(pid);
    
    // Удаляем старый pipe если существует
    unlink(clientPipeName.c_str());
    
    // Создаем новый pipe
    if (mkfifo(clientPipeName.c_str(), 0666) == -1) {
        std::cerr << "Ошибка создания клиентского pipe: " 
                 << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

void PipeClient::disconnect() {
    if (serverPipeFd != -1) {
        close(serverPipeFd);
        serverPipeFd = -1;
    }
    
    if (clientPipeFd != -1) {
        close(clientPipeFd);
        clientPipeFd = -1;
    }
    
    if (!clientPipeName.empty()) {
        unlink(clientPipeName.c_str());
        clientPipeName = "";
    }
}

bool PipeClient::sendMessage(const std::string& message) {
    if (serverPipeFd == -1) {
        return false;
    }
    
    ssize_t bytesWritten = write(serverPipeFd, message.c_str(), message.length());
    return bytesWritten == (ssize_t)message.length();
}

std::string PipeClient::receiveMessage(int timeoutMs) {
    // Открываем клиентский pipe для чтения если еще не открыт
    if (clientPipeFd == -1) {
        clientPipeFd = open(clientPipeName.c_str(), O_RDONLY | O_NONBLOCK);
        if (clientPipeFd == -1) {
            return "";
        }
    }
    
    // Используем select для таймаута
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(clientPipeFd, &readfds);
    
    struct timeval timeout;
    timeout.tv_sec = timeoutMs / 1000;
    timeout.tv_usec = (timeoutMs % 1000) * 1000;
    
    int ret = select(clientPipeFd + 1, &readfds, NULL, NULL, &timeout);
    
    if (ret > 0 && FD_ISSET(clientPipeFd, &readfds)) {
        char buffer[Constants::BUFFER_SIZE];
        ssize_t bytesRead = read(clientPipeFd, buffer, sizeof(buffer) - 1);
        
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            return std::string(buffer);
        }
    }
    
    return "";
}
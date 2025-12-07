#include "PipeClient.h"
#include "Constants.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>

PipeClient::PipeClient(const std::string& serverPipe): serverPipeName(serverPipe), clientPipeName(""), serverPipeFd(-1), clientPipeFd(-1) {}

PipeClient::~PipeClient()
{
    disconnect();
}

bool PipeClient::connectToServer()
{
    if (!createClientPipe()) return false;

    serverPipeFd = open(serverPipeName.c_str(), O_WRONLY);
    if (serverPipeFd == -1)
    {
        std::cerr << "Не удалось подключиться к серверу" << std::endl;
        return false;
    }

    return true;
}

bool PipeClient::createClientPipe() {
    // Используем PID для уникальности
    pid_t pid = getpid();
    clientPipeName = serverPipeName + "_response_" + std::to_string(pid);
    
    std::cout << "[CLIENT] Создаю response pipe: " << clientPipeName << std::endl;
    
    // Удаляем старый если есть
    unlink(clientPipeName.c_str());
    
    if (mkfifo(clientPipeName.c_str(), 0666) == -1) {
        std::cerr << "Ошибка создания pipe: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

void PipeClient::disconnect()
{
    if (serverPipeFd != -1)
    {
        close(serverPipeFd);
        serverPipeFd = -1;
    }

    if (clientPipeFd != -1)
    {
        close(clientPipeFd);
        clientPipeFd = -1;
    }

    if (!clientPipeName.empty())
    {
        unlink(clientPipeName.c_str());
        clientPipeName = "";
    }
}

bool PipeClient::sendMessage(const std::string& message) {
    if (serverPipeFd == -1) {
        std::cout << "[CLIENT ERROR] Не подключен к серверу!" << std::endl;
        return false;
    }
    
    std::cout << "[CLIENT] Пишу в серверный pipe " << serverPipeName 
              << ", fd=" << serverPipeFd << std::endl;
    std::cout << "[CLIENT] Сообщение: '" << message << "' (" << message.length() << " байт)" << std::endl;
    
    ssize_t bytesWritten = write(serverPipeFd, message.c_str(), message.length());
    
    std::cout << "[CLIENT] Записано " << bytesWritten << " байт" << std::endl;
    
    if (bytesWritten == -1) {
        std::cout << "[CLIENT ERROR] Ошибка записи: " << strerror(errno) << std::endl;
    }
    
    return bytesWritten == (ssize_t)message.length();
}

std::string PipeClient::receiveMessage(int timeoutMs)
{
    if (clientPipeFd == -1)
    {
        clientPipeFd = open(clientPipeName.c_str(), O_RDONLY | O_NONBLOCK);
        if (clientPipeFd == -1) return "";
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(clientPipeFd, &readfds);

    struct timeval timeout;
    timeout.tv_sec = timeoutMs / 1000;
    timeout.tv_usec = (timeoutMs % 1000) * 1000;

    int ret = select(clientPipeFd + 1, &readfds, NULL, NULL, &timeout);

    if (ret > 0 && FD_ISSET(clientPipeFd, &readfds))
    {
        char buffer[Constants::BUFFER_SIZE];
        ssize_t bytesRead = read(clientPipeFd, buffer, sizeof(buffer) - 1);

        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            return std::string(buffer);
        }
    }

    return "";
}

std::string PipeClient::getClientPipeName() const
{
    return clientPipeName;
}
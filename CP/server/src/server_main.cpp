#include <csignal>
#include <iostream>

#include "Server.h"

Server* serverInstance = nullptr;

void signalHandler(int signal) {
    std::cout << "\nПолучен сигнал " << signal << ", завершить работу..." << std::endl;
    if (serverInstance) {
        serverInstance->stop();
    }

    exit(0);
}

int main() {
    std::cout << "=== Сервер Морской бой ===" << std::endl;
    std::cout << "Для остановки нажмите Ctrl+C" << std::endl;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    std::string pipeName = "/tmp/battleship_server";

    Server server(pipeName);
    serverInstance = &server;

    try {
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
#include "Client.h"
#include <iostream>

int main() {
    std::cout << "=== Клиент Морской бой ===" << std::endl;
    
    std::string pipeName = "/tmp/battleship_server";
    
    Client client(pipeName);
    
    try {
        client.run();
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
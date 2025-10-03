#include <iostream>
#include <string>
#include "process.hpp"
#include "childProcess.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

ParentProcess::ParentProcess() {
    std::string file1, file2;
    std::cout << "Введите имя файла для child1: ";
    std::getline(std::cin, file1);
    std::cout << "Введите имя файла для child2: ";
    std::getline(std::cin, file2);

    Pipe* pipe1 = new Pipe();
    Pipe* pipe2 = new Pipe();

    child1 = new ChildProcess("child1", file1, pipe1);
    child2 = new ChildProcess("child2", file2, pipe2);

    delete pipe1;
    delete pipe2;
}

void ParentProcess::start() {
    std::cout << "Запуск дочерних процессов..." << std::endl;
    child1->execute();
    child2->execute();

#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif

    std::cout << "Введите строки для обработки (Ctrl+D/Ctrl+Z для завершения):" << std::endl;

    std::string line;
    int line_number = 0;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue; 
        
        line_number++;
        std::cout << "Обрабатывается строка " << line_number << ": " << line << std::endl;

#ifdef _WIN32
        DWORD written;
        if (line_number % 2 == 1) { 
            std::cout << "Отправка child1: " << line << std::endl;
            if (!WriteFile(child1->getReadFd(), line.c_str(), line.length(), &written, NULL)) {
                std::cerr << "Ошибка записи в child1" << std::endl;
            }
        } else { 
            std::cout << "Отправка child2: " << line << std::endl;
            if (!WriteFile(child2->getReadFd(), line.c_str(), line.length(), &written, NULL)) {
                std::cerr << "Ошибка записи в child2" << std::endl;
            }
        }
#else
        if (line_number % 2 == 1) { 
            std::cout << "Отправка child1: " << line << std::endl;
            write(child1->getReadFd(), line.c_str(), line.length());
        } else { 
            std::cout << "Отправка child2: " << line << std::endl;
            write(child2->getReadFd(), line.c_str(), line.length());
        }
#endif
    }

    std::cout << "Завершение работы..." << std::endl;

#ifdef _WIN32
    CloseHandle(child1->getReadFd());
    CloseHandle(child2->getReadFd());
#else
    close(child1->getReadFd());
    close(child2->getReadFd());
#endif

#ifdef _WIN32
    WaitForSingleObject(child1->getPid(), INFINITE);
    WaitForSingleObject(child2->getPid(), INFINITE);
    CloseHandle(child1->getPid());
    CloseHandle(child2->getPid());
#else
    int status;
    waitpid(child1->getPid(), &status, 0);
    waitpid(child2->getPid(), &status, 0);
#endif

    std::cout << "Родительский процесс завершен" << std::endl;
}

ParentProcess::~ParentProcess() {
    delete child1;
    delete child2;
}
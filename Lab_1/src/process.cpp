#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string>
#include "process.hpp"
#include "childProcess.hpp"

ParentProcess::ParentProcess() {
    std::string file1, file2;
    std::cout << "Введите имя файла для child1: ";
    std::getline(std::cin, file1);
    std::cout << "Введите имя файла для child2: ";
    std::getline(std::cin, file2);

    pipe1 = new Pipe();
    pipe2 = new Pipe();
    child1 = new ChildProcess(pipe1, file1, true);
    child2 = new ChildProcess(pipe2, file2, false);
}

void ParentProcess::start() {
    child1->execute();
    child2->execute();

    close(pipe1->getReadFd());
    close(pipe2->getReadFd());

    std::string line;
    int line_number = 0;

    while (std::getline(std::cin, line)) {
        line_number++;

        if (line_number % 2 == 1) { 
            write(pipe1->getWriteFd(), line.c_str(), line.length());
        } else { 
            write(pipe2->getWriteFd(), line.c_str(), line.length());
        }
    }

    close(pipe1->getWriteFd());
    close(pipe2->getWriteFd());

}

ParentProcess::~ParentProcess() {
    delete child1;
    delete child2;
    delete pipe1;
    delete pipe2;
}
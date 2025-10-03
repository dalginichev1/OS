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

void ParentProcess::start() 
{
    child1->execute();
    child2->execute();

    #ifdef _WIN32
    CloseHandle(pipe1->getReadFd());
    CloseHandle(pipe2->getReadFd());
    #else
    close(pipe1->getReadFd());
    close(pipe2->getReadFd());
    #endif

    std::string line;
    int line_number = 0;

    while (std::getline(std::cin, line)) 
    {
        line_number++;

        if (line_number % 2 == 1) 
        { 
            write(pipe1->getWriteFd(), line.c_str(), line.length());
        } else { 
            write(pipe2->getWriteFd(), line.c_str(), line.length());
        }
    }

    #ifdef _WIN_32
    CloseHandle(pipe1->getWriteFd());
    CloseHandle(pipe2->getWriteFd());
    #else
    close(pipe1->getWriteFd());
    close(pipe2->getWriteFd());
    #endif
}

ParentProcess::~ParentProcess() 
{
    #ifdef _WIN32
    WaitForSingleObject(child1->getPid(), INFINITE);
    WaitForSingleObject(child2->getPid(), INFINITE);
    CloseHandle(child1->getPid());
    CloseHandle(child2->getPid());
    #else
    waitpid(child1->getPid(), NULL, 0);
    waitpid(child2->getPid(), NULL, 0);
    #endif
    delete child1;
    delete child2;
    delete pipe1;
    delete pipe2;
}
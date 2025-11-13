#include "process.hpp"

#include <iostream>

#include <string>

void ParentProcess::start()
{
    child1->execute();
    child2->execute();

    sleep(10);

    std::string line;
    int line_number = 0;
    std::string data1, data2;

    while (std::getline(std::cin, line))
    {
        ++line_number;
        if (line_number % 2 == 1)
        {
            if (!data1.empty()) data1 += "\n";
            data1 += line;
        }
        else
        {
            if (!data2.empty()) data2 += "\n";
            data2 += line;
        }
    }

    shm1->writeData(data1);
    shm2->writeData(data2);

    std::cout << "Данные отправлены в дочерние процессы" << std::endl;

    shm1->waitConsumer();
    shm2->waitConsumer();

    std::cout << "Данные обработы дочерними процессами" << std::endl;

    int status1, status2;

    waitpid(child1->getPid(), &status1, 0);
    waitpid(child2->getPid(), &status2, 0);

    std::cout << "Дочерние процессы завершили работу" << std::endl;
}

ParentProcess::ParentProcess()
{
    std::cout << "Введите имя файла для child1: ";
    std::getline(std::cin, file1);
    std::cout << "Введите имя файла для child2: ";
    std::getline(std::cin, file2);

    shm1 = new SharedMemory("/child1_shm", 4096, true);
    shm2 = new SharedMemory("/child2_shm", 4096, true);

    child1 = new ChildProcess("/child1_shm", file1, true);
    child2 = new ChildProcess("/child2_shm", file2, false);
}

ParentProcess::~ParentProcess()
{
    delete child1;
    delete child2;
    delete shm1;
    delete shm2;
}
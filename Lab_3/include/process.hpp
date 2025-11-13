#pragma once

#include "sharedMemory.hpp"
#include "childProcess.hpp"
#include <string>

class ChildProcess;

class ParentProcess {
private:
    ChildProcess* child1;
    ChildProcess* child2;
    std::string file1;
    std::string file2;
    SharedMemory * shm1;
    SharedMemory * shm2;


public:
    ParentProcess();
    void start();
    ~ParentProcess();
};
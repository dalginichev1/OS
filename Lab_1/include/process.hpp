#pragma once
#include "pipe.hpp"
#include "childProcess.hpp"
#include <string>

class ChildProcess;

class ParentProcess {
private:
    Pipe* pipe1;
    Pipe* pipe2;
    ChildProcess* child1;
    ChildProcess* child2;

public:
    ParentProcess();
    void start();
    ~ParentProcess();
};
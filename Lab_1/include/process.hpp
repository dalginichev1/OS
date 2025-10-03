#pragma once
#include "pipe.hpp"
#include <string>

class ChildProcess;

class ParentProcess {
private:
    ChildProcess* child1;
    ChildProcess* child2;

public:
    ParentProcess();
    void start();
    ~ParentProcess();
};
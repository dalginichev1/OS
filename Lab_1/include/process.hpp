#pragma once
#include "pipe.hpp"
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

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
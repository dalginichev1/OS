#pragma once
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

class Pipe {
private:
#ifdef _WIN32
    HANDLE fd[2];
#else
    int fd[2];
#endif


public:
    Pipe();
    #ifdef _WIN_32
    HANDLE getReadFd() const;
    HANDLE getWriteFd() const;
    #else
    int getReadFd() const;
    int getWriteFd() const;
    #endif
    ~Pipe();
};
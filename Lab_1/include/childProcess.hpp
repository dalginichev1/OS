#pragma once
#include "pipe.hpp"
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

class ChildProcess {
private:
#ifdef _WIN32
    HANDLE pid;
    HANDLE hReadFd;  // Изменили на дескриптор чтения
#else
    pid_t pid;
    int read_fd;     // Изменили на дескриптор чтения
#endif
    std::string file_name;
    std::string prefix;

public:
    ChildProcess(const std::string& prefix, const std::string& fname, Pipe* pipe);
    void execute();
#ifdef _WIN32
    HANDLE getPid() const;
    HANDLE getReadFd() const;  // Изменили на getReadFd
#else
    pid_t getPid() const;
    int getReadFd() const;     // Изменили на getReadFd
#endif
};
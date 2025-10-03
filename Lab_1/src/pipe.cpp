#include <iostream>
#include "pipe.hpp"

Pipe::Pipe() 
{
    #ifdef _WIN32
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    if (!CreatePipe(&fd[0], &fd[1], &saAttr, 0))
    {
        std::cerr << "Fail in create pipe";
        exit(1);
    }
    #else
    if (pipe(fd) == -1) 
    {
        perror("Создание канала не удалось");
        exit(1);
    }
    #endif
}
#ifdef _WIN32
HANDLE Pipe::getReadFd() const
{
    return fd[0];
}
HANDLE Pipe::getWriteFd() const
{
    return fd[1];
}
#else
int Pipe::getReadFd() const
{
    return fd[0];
}
int Pipe::getWriteFd() const 
{
    return fd[1];
}
#endif

Pipe::~Pipe() {
    #ifdef _WIN_32
    CloseHandle(fd[0]);
    CloseHandle(fd[1]);
    #else
    close(fd[0]);
    close(fd[1]);
    #endif
}
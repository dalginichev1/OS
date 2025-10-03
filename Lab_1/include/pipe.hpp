#pragma once

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

class Pipe {
private:
#ifdef _WIN32
    HANDLE read_fd;
    HANDLE write_fd;
#else
    int fd[2]; 
#endif

public:
    Pipe();
    int getReadFd() const;
    int getWriteFd() const;
    ~Pipe();
};
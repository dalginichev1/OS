#include <iostream>
#include <cstdlib>
#include "pipe.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

Pipe::Pipe() {
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&read_fd, &write_fd, &sa, 0)) {
        std::cerr << "Создание канала не удалось" << std::endl;
        exit(1);
    }
#else
    if (pipe(fd) == -1) {
        perror("Создание канала не удалось");
        exit(1);
    }
#endif
}

#ifdef _WIN32
HANDLE Pipe::getReadFd() const {
    return read_fd;
}

HANDLE Pipe::getWriteFd() const {
    return write_fd;
}
#else
int Pipe::getReadFd() const {
    return fd[0];
}

int Pipe::getWriteFd() const {
    return fd[1];
}
#endif

Pipe::~Pipe() {
#ifdef _WIN32
    CloseHandle(read_fd);
    CloseHandle(write_fd);
#else
    close(fd[0]);
    close(fd[1]);
#endif
}
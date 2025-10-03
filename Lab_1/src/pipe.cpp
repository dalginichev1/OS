#include <iostream>
#include <cstdlib>
#include "pipe.hpp"

#ifdef _WIN32
#include <io.h>
#define close _close
#define read _read
#define write _write
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

int Pipe::getReadFd() const {
#ifdef _WIN32
    return (int)read_fd;
#else
    return fd[0];
#endif
}

int Pipe::getWriteFd() const {
#ifdef _WIN32
    return (int)write_fd;
#else
    return fd[1];
#endif
}

Pipe::~Pipe() {
#ifdef _WIN32
    CloseHandle(read_fd);
    CloseHandle(write_fd);
#else
    close(fd[0]);
    close(fd[1]);
#endif
}
#include <iostream>
#include <unistd.h>
#include "pipe.hpp"

Pipe::Pipe() {
    if (pipe(fd) == -1) {
        perror("Создание канала не удалось");
        exit(1);
    }
}

int Pipe::getReadFd() const {
    return fd[0];
}

int Pipe::getWriteFd() const {
    return fd[1];
}

Pipe::~Pipe() {
    close(fd[0]);
    close(fd[1]);
}
#include <unistd.h>
#include <string>
#include <iostream>
#include "childProcess.hpp"

ChildProcess::ChildProcess(Pipe* p, const std::string& f, bool is_c1) 
    : pipe(p), file_name(f), is_child1(is_c1), pid(-1) {}

void ChildProcess::execute() {
    pid = fork();
    if (pid == -1) {
        perror("Не удалось создать процесс");
        exit(1);
    }

    if (pid == 0) {
        close(pipe->getWriteFd());
        
        std::string read_fd_str = std::to_string(pipe->getReadFd());
        std::string prefix = is_child1 ? "child1" : "child2";
        
        execl("./child", "./child", prefix.c_str(), file_name.c_str(), 
              read_fd_str.c_str(), nullptr);
        
        perror("Не удалось запустить дочернюю программу");
        exit(1);
    }
    else
    {
        close(pipe->getReadFd());
    }
}

pid_t ChildProcess::getPid() const {
    return pid;
}
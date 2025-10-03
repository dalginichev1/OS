#include <string>
#include <iostream>
#include <cstdlib>
#include "childProcess.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
#endif

ChildProcess::ChildProcess(Pipe* p, const std::string& f, bool is_c1) 
    : pipe(p), file_name(f), is_child1(is_c1), pid(-1) {}

void ChildProcess::execute() {
#ifdef _WIN32
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string read_fd_str = std::to_string((int)pipe->getReadFd());
    std::string prefix = is_child1 ? "child1" : "child2";
    
    std::string command = "child.exe " + prefix + " " + file_name + " " + read_fd_str;
    
    if (!CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, 
                       TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Не удалось создать процесс" << std::endl;
        exit(1);
    }
    
    pid = pi.dwProcessId;
    CloseHandle(pi.hThread);
#else
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
    else {
        close(pipe->getReadFd());
    }
#endif
}

#ifdef _WIN32
DWORD ChildProcess::getPid() const {
    return pid;
}
#else
pid_t ChildProcess::getPid() const {
    return pid;
}
#endif
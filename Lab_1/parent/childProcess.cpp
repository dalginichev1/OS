#include <iostream>
#include <cstdlib>
#include <string>
#include "childProcess.hpp"

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

ChildProcess::ChildProcess(const std::string& pref, const std::string& fname, Pipe* pipe) 
    : prefix(pref), file_name(fname) 
{
#ifdef _WIN32
    pid = NULL;
    hReadFd = pipe->getReadFd();
    CloseHandle(pipe->getWriteFd());
#else
    pid = -1;
    read_fd = pipe->getReadFd();
    close(pipe->getWriteFd());
#endif
}

void ChildProcess::execute() {
#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string cmd = "child.exe " + prefix + " " + file_name + " " + 
                     std::to_string((intptr_t)hReadFd);

    std::cout << "Запускаем дочерний процесс: " << prefix << std::endl;

    if (!CreateProcess(NULL, 
                      const_cast<LPSTR>(cmd.c_str()),
                      NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Не удалось создать процесс: " << GetLastError() << std::endl;
        exit(1);
    }
    pid = pi.hProcess;
    CloseHandle(pi.hThread);
#else
    pid = fork();
    if (pid == -1) {
        perror("Не удалось создать процесс");
        exit(1);
    }

    if (pid == 0) { 
        std::string read_fd_str = std::to_string(read_fd);
        std::cout << "Дочерний процесс " << prefix << " запущен" << std::endl;
        execl("./child", "child", prefix.c_str(), file_name.c_str(), 
              read_fd_str.c_str(), NULL);
        
        perror("Не удалось запустить дочернюю программу");
        exit(1);
    } else {
        close(read_fd);
        std::cout << "Родительский процесс: создан дочерний " << prefix 
                  << " с PID: " << pid << std::endl;
    }
#endif
}

#ifdef _WIN32
HANDLE ChildProcess::getPid() const {
    return pid;
}

HANDLE ChildProcess::getReadFd() const {
    return hReadFd;
}
#else
pid_t ChildProcess::getPid() const {
    return pid;
}

int ChildProcess::getReadFd() const {
    return read_fd;
}
#endif
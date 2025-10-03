#pragma once
#include "pipe.hpp"
#include <string>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
typedef HANDLE pid_t;
typedef HANDLE fd_t;
#else
#include <unistd.h>
#include <sys/wait.h>
typedef pid_t pid_t;

#include <fcntl.h>
#endif

class ChildProcess {
private:
    pid_t pid;
    Pipe* pipe;
    std::string file_name;
    bool is_child1;

public:
    ChildProcess(Pipe* p, const std::string& f, bool is_c1);
    #ifdef _WIN32
    void launch_child(const std::string& child_bin, const std::string& prefix, const std::string& file_name, fd_t read_fd, pid_t& pid );
    #else
    void launch_child(const std::string& child_bin, const std::string& prefix, const std::string& file_name, int read_fd, pid_t& pid );
    #endif
    void execute();
    pid_t getPid() const;
};

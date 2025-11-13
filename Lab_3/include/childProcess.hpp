#pragma once

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

#include <string>


class ChildProcess {
private:
    pid_t pid;
    std::string shm_name;
    std::string prefix;
    std::string output_file_name;
    bool is_child1;

public:
    ChildProcess(const std::string& shm_name, const std::string& f, bool is_c1);
    void execute();
    pid_t getPid() const;
};
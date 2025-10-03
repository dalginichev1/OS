#pragma once
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif
#include <string>

class ChildProcessor {
private:
    char buffer[256];
    std::string prefix;
    std::string reverseString(const std::string& s);

public:
    ChildProcessor(const std::string& prefix);
    int process(int read_fd, const std::string& output_file);
};
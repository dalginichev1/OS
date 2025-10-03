#pragma once
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

class ChildProcessor {
private:
    char buffer[256];
    std::string prefix;
    std::string reverseString(const std::string& s);

public:
    ChildProcessor(const std::string& prefix);
#ifdef _WIN32
    int process(HANDLE read_fd, const std::string& output_file);
#else
    int process(int read_fd, const std::string& output_file);
#endif
};
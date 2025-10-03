#pragma once
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
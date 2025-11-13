#pragma once
#include <string>

class ChildProcessor {
private:
    std::string prefix;
    std::string reverseString(const std::string& s);

public:
    ChildProcessor(const std::string& prefix);
    int process(const std::string& shm_name, const std::string& output_file);
};
#include <iostream>
#include <fstream>
#include <string>
#include "childProcessor.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

ChildProcessor::ChildProcessor(const std::string& pref) : prefix(pref) {}

std::string ChildProcessor::reverseString(const std::string& s) {
    std::string result;
    for (size_t i = s.length(); i > 0; --i) {
        result += s[i - 1];
    }
    return result;
}

#ifdef _WIN32
int ChildProcessor::process(HANDLE read_fd, const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << output_file << std::endl;
        return 1;
    }

    DWORD count;
    while (ReadFile(read_fd, buffer, sizeof(buffer) - 1, &count, NULL) && count > 0) {
        buffer[count] = '\0';
        std::string line(buffer);
        std::string reversed = reverseString(line);
        file << prefix << ": " << reversed << std::endl;
    }

    CloseHandle(read_fd);
    file.close();
    return 0;
}
#else
int ChildProcessor::process(int read_fd, const std::string& output_file) {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << output_file << std::endl;
        return 1;
    }

    ssize_t count;
    while ((count = read(read_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[count] = '\0';
        std::string line(buffer);
        std::string reversed = reverseString(line);
        file << prefix << ": " << reversed << std::endl;
    }

    close(read_fd);
    file.close();
    return 0;
}
#endif
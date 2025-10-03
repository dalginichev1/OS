#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include "childProcessor.hpp"

ChildProcessor::ChildProcessor(const std::string& pref) : prefix(pref) {}

std::string ChildProcessor::reverseString(const std::string& s) {
    size_t len = s.length();
    std::string result;
    for (size_t i = len; i > 0; --i) {
        result += s[i - 1];
    }
    return result;
}

int ChildProcessor::process(int read_fd, const std::string& output_file) {
    int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Не удалось открыть файл для записи");
        return 1;
    }

    ssize_t count;
    while ((count = read(read_fd, buffer, sizeof(buffer) - 1)) > 0) 
    {
        buffer[count] = '\0';
        std::string line(buffer);
        std::string reversed = reverseString(line);
        write(fd, reversed.c_str(), reversed.length());
    }

    if (count == -1) {
        perror("Ошибка чтения из канала");
    }

    close(read_fd);
    close(fd);
    return 0;
}
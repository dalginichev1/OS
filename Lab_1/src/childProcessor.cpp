#include <iostream>
#include <string>
#include <cstdlib>
#include "childProcessor.hpp"

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
    #define close _close
    #define read _read
    #define write _write
    #define open _open
    #define O_WRONLY _O_WRONLY
    #define O_CREAT _O_CREAT
    #define O_TRUNC _O_TRUNC
#else
    #include <unistd.h>
    #include <fcntl.h>
#endif

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
#ifdef _WIN32
    int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, _S_IREAD | _S_IWRITE);
#else
    int fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
    
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
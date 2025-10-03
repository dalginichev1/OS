#include <iostream>
#include <string>
#include "childProcessor.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <cstdlib>
#endif

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Использование: " << argv[0] << " <prefix> <output_file> <read_fd>" << std::endl;
        return 1;
    }

    std::string prefix = argv[1];
    std::string output_file = argv[2];

#ifdef _WIN32
    // В Windows передаем дескриптор через временный pipe
    HANDLE temp_pipe = (HANDLE)(intptr_t)std::stoll(argv[3]);
    HANDLE read_fd;
    DWORD read;
    if (!ReadFile(temp_pipe, &read_fd, sizeof(read_fd), &read, NULL)) {
        std::cerr << "Ошибка получения дескриптора" << std::endl;
        return 1;
    }
    CloseHandle(temp_pipe);
#else
    int read_fd = std::stoi(argv[3]);
#endif

    std::cout << prefix << ": Дочерний процесс запущен" << std::endl;
    std::cout << prefix << ": Будет писать в файл: " << output_file << std::endl;

    ChildProcessor processor(prefix);
    int result = processor.process(read_fd, output_file);

    std::cout << prefix << ": Дочерний процесс завершил работу с кодом: " << result << std::endl;
    return result;
}
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
    // В Windows просто преобразуем переданное число обратно в HANDLE
    HANDLE read_fd = reinterpret_cast<HANDLE>(std::stoll(argv[3]));
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
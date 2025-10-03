#include <iostream>
#include <cstdlib>
#include <string>
#include "childProcessor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Использование: " << argv[0] << " <prefix> <output_file> <read_fd>" << std::endl;
        return 1;
    }

    std::string prefix = argv[1];
    std::string output_file = argv[2];
    int read_fd = std::atoi(argv[3]);

    std::cout << prefix << ": запущен с файлом " << output_file << std::endl;

    ChildProcessor processor(prefix);
    return processor.process(read_fd, output_file);
}
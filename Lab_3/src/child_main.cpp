#include <iostream>
#include <cstdlib>
#include <string>
#include "childProcessor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Использование: " << argv[0] << " <prefix> <shm_name> <output_file>" << std::endl;
        return 1;
    }

    std::string prefix = argv[1];
    std::string shm_name = argv[2];
    std::string output_file = argv[3];

    std::cout << prefix << ": запущен с shared memory " << shm_name 
              << " и выходным файлом " << output_file << std::endl;

    ChildProcessor processor(prefix);
    return processor.process(shm_name, output_file);
}
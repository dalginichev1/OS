#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include "childProcessor.hpp"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <read_fd> <output_file> <prefix>" << std::endl;
        return 1;
    }

    int read_fd = std::stoi(argv[1]);
    std::string output_file = argv[2];
    std::string prefix = argv[3];

    ChildProcessor processor(prefix);
    return processor.process(read_fd, output_file);
}
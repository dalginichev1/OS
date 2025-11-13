#include "childProcessor.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>

#include "sharedMemory.hpp"

ChildProcessor::ChildProcessor(const std::string& pref) : prefix(pref) {}

std::string ChildProcessor::reverseString(const std::string& s) {
    size_t len = s.length();
    std::string result;
    for (size_t i = len; i > 0; --i) {
        result += s[i - 1];
    }
    return result;
}

int ChildProcessor::process(const std::string& shm_name, const std::string& output_file)
{
    SharedMemory shm(shm_name, 4096, false);
    std::string data = shm.readData();

    if (data.empty()) {
        std::cerr << prefix << ": нет данных" << std::endl;
        return 1;
    }

    std::cout << prefix << ": получено '" << data << "'" << std::endl;

    std::ofstream outfile(output_file);
    if (!outfile) {
        std::cerr << prefix << ": не удалось создать файл" << std::endl;
        return 1;
    }

    std::istringstream stream(data);
    std::string line;
    int line_count = 0;

    while (std::getline(stream, line)) {
        if (!line.empty()) {
            std::string reversed = reverseString(line);
            outfile << reversed << std::endl;
            line_count++;
        }
    }

    std::cout << prefix << ": записано " << line_count << " строк в " << output_file << std::endl;
    return 0;
}
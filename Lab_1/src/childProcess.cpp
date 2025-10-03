#include <unistd.h>
#include <string>
#include "childProcess.hpp"
#include "childProcessor.hpp"

ChildProcess::ChildProcess(Pipe* p, const std::string& f, bool is_c1) 
    : pipe(p), file_name(f), is_child1(is_c1), pid(-1) {}

void ChildProcess::execute() {
    pid = fork();
    if (pid == -1) {
        perror("Не удалось создать процесс");
        exit(1);
    }

    if (pid == 0) { 
        close(pipe->getWriteFd());
        std::string prefix = is_child1 ? "child1" : "child2";
        ChildProcessor processor(prefix);
        processor.process(pipe->getReadFd(), file_name);
        exit(0);
    }
}

pid_t ChildProcess::getPid() const {
    return pid;
}
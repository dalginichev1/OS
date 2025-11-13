#include "childProcess.hpp"

ChildProcess::ChildProcess(const std::string& shm_n, const std::string& f, bool is_c1)
    : shm_name(shm_n), output_file_name(f), is_child1(is_c1), pid(-1)
{
    prefix = is_child1 ? "child1" : "child2";
}

void ChildProcess::execute()
{
    pid = fork();
    if (pid == -1)
    {
        perror("Не удалось создать процесс");
        exit(1);
    }

    if (pid == 0)
    {
        execl("./child", "./child", prefix.c_str(), shm_name.c_str(), output_file_name.c_str(), nullptr);

        perror("Не удалось запустить дочерний процесс");
        exit(1);
    }
}

pid_t ChildProcess::getPid() const
{
    return pid;
}
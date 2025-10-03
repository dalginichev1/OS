#include <unistd.h>
#include <string>
#include "childProcess.hpp"
#include "childProcessor.hpp"

ChildProcess::ChildProcess(Pipe* p, const std::string& f, bool is_c1) 
    : pipe(p), file_name(f), is_child1(is_c1), pid(-1) {}

#ifdef _WIN32
void ChildProcess::launch_child(const std::string& child_bin, const std::string& prefix, const std::string& file_name, HANDLE read_fd, pid_t& pid)
{
    STARTUPINFO start_inf;
    PROCESS_INFORMATION process_inf;
    ZeroMemory(&start_inf, sizeof(start_inf));
    start_inf.cb = sizeof(start_inf);
    ZeroMemory(&process_inf, sizeof(process_inf));

    SetHandleInformation(read_fd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

    std::string cmd = child_bin + " " + prefix + " " + file_name + " " + std::to_string(reinterpret_cast<intptr_t>(read_fd));

    if (!CreateProcess(NULL, const_cast<char*>(cmd.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        std::cerr << "Fail in create proccess" std::endl;
        exit(1);
    }
    pid = process_inf.hProcess;
    CloseHandle(process_inf.hProcess); 
}
#else
void ChildProcess::launch_child(const std::string& child_bin, const std::string& prefix, const std::string& file_name, int read_fd, pid_t& pid)
{
    pid = fork();
    if (pid == -1) {
        perror("Не удалось создать процесс");
        exit(1);
    }

    if (pid == 0) { 
        close(read_fd);

        std::string fd_str = std::to_string(read_fd);

        execlp(child_bin.c_str(), child_bin.c_str(), prefix.c_str(), file_name.c_str(), fd_str.c_str(), NULL);
        exit(0);
    }
}
#endif

void ChildProcess::execute() 
{
    std::string prefix = is_child1 ? "child1" : "child2";
    std::string child_bin = 
    #ifdef _WIN32
    "child.exe";
    #else
    "./child";
    #endif

    launch_child(child_bin, prefix, file_name, pipe->getReadFd(), pid);
}

pid_t ChildProcess::getPid() const {
    return pid;
}
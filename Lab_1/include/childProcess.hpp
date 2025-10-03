#pragma once
#include "pipe.hpp"
#include "string"

#ifdef _WIN32
    #include <windows.h>
    typedef DWORD process_id;
#else
    #include <unistd.h>
    typedef pid_t process_id;
#endif

class ChildProcess {
private:
    process_id pid;
    Pipe* pipe;
    std::string file_name;
    bool is_child1;

public:
    ChildProcess(Pipe* p, const std::string& f, bool is_c1);
    void execute();
    process_id getPid() const;
};
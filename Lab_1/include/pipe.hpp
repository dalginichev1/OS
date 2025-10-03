#pragma once

class Pipe {
private:
    int fd[2]; 

public:
    Pipe();
    int getReadFd() const;
    int getWriteFd() const;
    ~Pipe();
};
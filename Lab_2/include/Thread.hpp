#pragma once

#include <pthread.h>

class Thread
{
    private:
        pthread_t tid;
        bool created;
    public:
        Thread();
        ~Thread();

        int create(void* (*start_routine)(void*), void* arg);
        int join(void** retval);
};
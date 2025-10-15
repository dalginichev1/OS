#include "Thread.hpp"

Thread::Thread(): created(false) {}

Thread::~Thread() {}


int Thread::create(void* (*start_routine)(void*), void* arg)
{
    int result = pthread_create(&tid, nullptr, start_routine, arg);
    if (result == 0)
    {
        created = true;
    }

    return result;
}

int Thread::join(void** retval)
{
    if (!created)
    {
        return -1;
    }

    return pthread_join(tid, retval);
}
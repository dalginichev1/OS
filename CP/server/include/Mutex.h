#pragma once

#include <pthread.h>

class Mutex
{
    private:
        pthread_mutex_t mutex;

    public:
        Mutex() {pthread_mutex_init(&mutex, NULL);}
        ~Mutex() {pthread_mutex_destroy(&mutex);}

        void lock() {pthread_mutex_lock(&mutex);}
        void unlock() {pthread_mutex_unlock(&mutex);}
        bool try_lock() {return pthread_mutex_trylock(&mutex) == 0;}

        pthread_mutex_t* native_handle() { return &mutex; }
};

class LockGuard
{
    private:
        pthread_mutex_t * mutex;

    public:
        explicit LockGuard(Mutex& mtx)
        {
            mutex = mtx.native_handle();
            pthread_mutex_lock(mutex);
        }
        explicit LockGuard(pthread_mutex_t * mtx): mutex(mtx)
        {
            pthread_mutex_lock(mutex);
        }

        ~LockGuard() {if (mutex) pthread_mutex_unlock(mutex);}
};
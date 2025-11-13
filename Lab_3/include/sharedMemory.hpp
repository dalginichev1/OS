#pragma once

#include <string>

class SharedMemory 
{
    private:
        std::string name;
        int fd;
        void * data;
        size_t size;
        bool is_owner;

        struct Semaphore
        {
            void * sem_data;
        };

        Semaphore sem_prod;
        Semaphore sem_cons;
    public:
        SharedMemory(const std::string& name, size_t size, bool shm_create = false);
        ~SharedMemory() noexcept;

        void * getData() const;
        size_t getSize() const;
        void writeData(const std::string& datas);
        std::string readData();

        void waitProducer();
        void waitConsumer();
        void postProducer();
        void postConsumer();
};
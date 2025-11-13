#include "sharedMemory.hpp"

#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <semaphore.h>

SharedMemory::SharedMemory(const std::string& shm_name, size_t shm_size, bool shm_create)
    : name(shm_name), size(shm_size), is_owner(shm_create) 
{
    if (shm_create)
    {
        fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if (fd == -1)
        {
            perror("shm create error");
            exit(1);
        }

        size_t total_size = size + sizeof(sem_t) * 2;
        if (ftruncate(fd, total_size) == -1)
        {
            perror("Не удалось установить размер");
            exit(1);
        }
    }
    else
    {
        fd = shm_open(name.c_str(), O_RDWR, 0666);
        if (fd == - 1)
        {
            perror("shm open error");
            exit(1);
        }
    }

    data = mmap(NULL, size + sizeof(sem_t) * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
    {
        perror("mmap error");
        exit(1);
    }
    
    sem_prod.sem_data = static_cast<char*>(data) + size;
    sem_cons.sem_data = static_cast<char*>(data) + size + sizeof(sem_t);

    if (shm_create)
    {
        if (sem_init(static_cast<sem_t*>(sem_prod.sem_data), 1, 0) == -1)
        {
            perror("sem_init producer error");
            exit(1);
        }
        if (sem_init(static_cast<sem_t*>(sem_cons.sem_data), 1, 0) == -1)
        {
            perror("sem_init consumer error");
            exit(1);
        }
    }
}

void SharedMemory::writeData(const std::string& datas)
{
    if (datas.size() >= size)
    {
        std::cerr << "Слишком большой объём данных для shared memory" << std::endl;
    }

    size_t data_size = datas.size();
    memcpy(data, &data_size, sizeof(size_t));
    memcpy(static_cast<char*>(data) + sizeof(size_t), datas.c_str(), data_size);

    postProducer();
}

std::string SharedMemory::readData()
{
    waitProducer();

    size_t data_size;
    memcpy(&data_size, data, sizeof(size_t));

    if (data_size >= size - sizeof(size_t))
    {
        return "";
    }

    std::string result(static_cast<char*>(data) + sizeof(size_t), data_size);

    postConsumer();

    return result;
}

void SharedMemory::waitProducer()
{
    sem_wait(static_cast<sem_t*>(sem_prod.sem_data));
}

void SharedMemory::waitConsumer()
{
    sem_wait(static_cast<sem_t*>(sem_cons.sem_data));
}

void SharedMemory::postProducer()
{
    sem_post(static_cast<sem_t*>(sem_prod.sem_data));
}

void SharedMemory::postConsumer()
{
    sem_post(static_cast<sem_t*>(sem_cons.sem_data));
}

SharedMemory::~SharedMemory()
{
    if (data != MAP_FAILED)
    {
        if (is_owner)
        {
            sem_destroy(static_cast<sem_t*>(sem_prod.sem_data));
            sem_destroy(static_cast<sem_t*>(sem_cons.sem_data));
        }

        munmap(data, size + sizeof(sem_t) * 2);
    }

    if (fd != -1)
    {
        close(fd);
    }

    if (is_owner)
    {
        shm_unlink(name.c_str());
    }
}

void * SharedMemory::getData() const
{
    return data;
}

size_t SharedMemory::getSize() const
{
    return size;
}
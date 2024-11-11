#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <vector>
#include <mutex>
#include <thread>

class SharedMemory {
    public:
        SharedMemory(size_t size);

        uint64_t read(int address);
        void write(int address, uint64_t value);

    private:
        std::vector<uint64_t> _data; // Vector que simula la memoria con datos de 64 bits
        std::mutex _mutex;           // Mutex para sincronizar el acceso a la memoria
};

#endif // SHARED_MEMORY_H
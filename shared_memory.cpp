#include "shared_memory.h"
#include <iostream>

SharedMemory::SharedMemory(size_t size) : _data(size) {}

uint64_t SharedMemory::read(int address) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (address < 0 || address >= _data.size()) {
        std::cerr << "Error: Address out ouf bounds" << std::endl;
        return 0;
    } 
    return _data[address];
}

void SharedMemory::write(int address, uint64_t value) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (address < 0 || address >= _data.size()) {
        std::cerr << "Error: Address out of bounds" << std::endl;
    }
    _data[address] = value;
}
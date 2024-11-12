// shared_memory.cpp
#include "shared_memory.h"

SharedMemory::SharedMemory(size_t size) : _data(size) {}

uint64_t SharedMemory::read(int address) {
  std::lock_guard<std::mutex> lock(_mutex);
  if (address < 0 || address >= static_cast<int>(_data.size())) {
    // Manejo de errores
    return 0;
  }
  return _data[address];
}

void SharedMemory::write(int address, uint64_t value) {
  std::lock_guard<std::mutex> lock(_mutex);
  if (address < 0 || address >= static_cast<int>(_data.size())) {
    // Manejo de errores
    return;
  }
  _data[address] = value;
}

void SharedMemory::reset() {
  std::lock_guard<std::mutex> lock(_mutex);
  std::fill(_data.begin(), _data.end(), 0);
}

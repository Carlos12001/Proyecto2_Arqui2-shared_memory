// shared_memory.h
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <mutex>
#include <vector>

class SharedMemory {
 public:
  SharedMemory(size_t size);
  uint64_t read(int address);
  void write(int address, uint64_t value);
  void reset();  // Agrega esta línea

 private:
  std::vector<uint64_t> _data;
  std::mutex _mutex;
};

#endif  // SHARED_MEMORY_H

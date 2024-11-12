// cache.h
#ifndef CACHE_H
#define CACHE_H

#include <mutex>
#include <vector>

#include "bus.h"
#include "shared_memory.h"

enum class MESIState { Modified, Exclusive, Shared, Invalid };

// Estructura para representar una línea de caché
struct CacheBlock {
  uint64_t data;
  bool valid;
  int tag;
  MESIState state;

  CacheBlock() : data(0), valid(false), tag(-1), state(MESIState::Invalid) {}
};

class Bus;  // Declaración anticipada

class Cache {
 public:
  Cache(int numBlocks, SharedMemory *sharedMem);

  uint64_t load(int addr, Bus *bus);
  void store(int addr, uint64_t value, Bus *bus);

  void snoopBusRd(int addr);
  void snoopBusRdX(int addr);
  void snoopBusUpgr(int addr);
  void reset();  // Agrega esta línea
  int getNumBlocks() const;
  CacheBlock getBlock(int index) const;

 private:
  std::vector<CacheBlock> blocks;
  std::mutex cache_mutex;
  SharedMemory *sharedMemory;

  // Métodos auxiliares
  int findBlock(int addr) const;
  void fetchFromMemory(int addr, int index);
};

#endif  // CACHE_H

// cache.cpp
#include "cache.h"

#include "bus.h"

Cache::Cache(int numBlocks, SharedMemory *sharedMem)
    : blocks(numBlocks), sharedMemory(sharedMem) {}

void Cache::reset() {
  std::lock_guard<std::mutex> lock(cache_mutex);
  for (auto &block : blocks) {
    block = CacheBlock();
  }
}

int Cache::findBlock(int addr) const { return addr % blocks.size(); }

void Cache::fetchFromMemory(int addr, int index) {
  blocks[index].data = sharedMemory->read(addr);
  blocks[index].valid = true;
  blocks[index].tag = addr;
  blocks[index].state = MESIState::Exclusive;
}

uint64_t Cache::load(int addr, Bus *bus) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state != MESIState::Invalid) {
    // Cache hit
    return block.data;
  } else {
    // Cache miss
    bus->sendBusRd(addr, this);

    // Fetch data from memory
    fetchFromMemory(addr, index);
    return blocks[index].data;
  }
}

void Cache::store(int addr, uint64_t value, Bus *bus) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (!(block.valid && block.tag == addr &&
        block.state != MESIState::Invalid)) {
    // Cache miss
    bus->sendBusRdX(addr, this);

    // Fetch data from memory
    fetchFromMemory(addr, index);
  } else if (block.state == MESIState::Shared) {
    // Upgrade to Modified state
    bus->sendBusUpgr(addr, this);
  }

  block.data = value;
  block.state = MESIState::Modified;
  sharedMemory->write(addr, value);  // Write-through for simplicity
}

// Implementación de los métodos de snooping
void Cache::snoopBusRd(int addr) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state != MESIState::Invalid) {
    // La caché tiene una copia válida del bloque
    if (block.state == MESIState::Modified) {
      // Escribimos el valor de vuelta a la memoria
      sharedMemory->write(addr, block.data);
      block.state = MESIState::Shared;
    } else if (block.state == MESIState::Exclusive) {
      block.state = MESIState::Shared;
    }
    // Si el estado es Shared, no hacemos nada
  }
}

void Cache::snoopBusRdX(int addr) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state != MESIState::Invalid) {
    if (block.state == MESIState::Modified) {
      // Escribimos el valor de vuelta a la memoria
      sharedMemory->write(addr, block.data);
    }
    // Invalida nuestra copia
    block.state = MESIState::Invalid;
    block.valid = false;
  }
}

void Cache::snoopBusUpgr(int addr) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state == MESIState::Shared) {
    // Invalida nuestra copia
    block.state = MESIState::Invalid;
    block.valid = false;
  }
}

// Implementación de getNumBlocks()
int Cache::getNumBlocks() const { return blocks.size(); }

// Implementación de getBlock()
CacheBlock Cache::getBlock(int index) const {
  // Opcionalmente, puedes agregar verificación de límites
  if (index >= 0 && index < static_cast<int>(blocks.size())) {
    return blocks[index];
  } else {
    // Manejo de acceso fuera de los límites
    return CacheBlock();  // Retorna un bloque de caché inválido por defecto
  }
}

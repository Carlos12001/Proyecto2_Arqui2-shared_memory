// cache.cpp
#include "cache.h"

#include <iostream>

#include "bus.h"

Cache::Cache(int numBlocks, SharedMemory *sharedMem)
    : blocks(numBlocks), sharedMemory(sharedMem) {}

int Cache::findBlock(int addr) const {
  int index = addr % blocks.size();
  return index;
}

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

void Cache::snoopBusRd(int addr) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state != MESIState::Invalid) {
    if (block.state == MESIState::Modified) {
      // Write back to memory
      sharedMemory->write(addr, block.data);
    }
    block.state = MESIState::Shared;
  }
}

void Cache::snoopBusRdX(int addr) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state != MESIState::Invalid) {
    if (block.state == MESIState::Modified) {
      // Write back to memory
      sharedMemory->write(addr, block.data);
    }
    block.state = MESIState::Invalid;
    block.valid = false;
  }
}

void Cache::snoopBusUpgr(int addr) {
  std::lock_guard<std::mutex> lock(cache_mutex);
  int index = findBlock(addr);
  CacheBlock &block = blocks[index];

  if (block.valid && block.tag == addr && block.state == MESIState::Shared) {
    block.state = MESIState::Invalid;
    block.valid = false;
  }
}

int Cache::getNumBlocks() const { return blocks.size(); }

CacheBlock Cache::getBlock(int index) const { return blocks[index]; }

// cache.cpp
void Cache::reset() {
  std::lock_guard<std::mutex> lock(cache_mutex);
  for (auto &block : blocks) {
    block = CacheBlock();
  }
}

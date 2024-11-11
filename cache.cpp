#include "cache.h"
#include <iostream>


Cache::Cache(int numBlocks) : blocks(numBlocks) {}

uint64_t Cache::load(int addr) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    int index = addr % blocks.size();

    CacheBlock& block = blocks[index];

    if (block.valid && block.tag == addr) {
        //if (block.state == MESIState::Modified || block.state == MESIState::Exclusive || block.state == MESIState::Shared) {
            return block.data; // Cache hit
        //}
    } else {
        // Cache miss
        std::cerr << "Cache miss at address" << addr << std::endl;
        //block.state = MESIState::Exclusive; // Asignar estado luego de cargar de memoria
        return 0;
    }
}


void Cache::store(int addr, uint64_t value) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    int index = addr % blocks.size();
    CacheBlock& block = blocks[index];

    block.data = value;
    block.valid = true;
    block.tag = addr;
    block.state = MESIState::Modified; // Es modificado al escribir
}

void Cache::snoopBusRd(int addr) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    int index = addr % blocks.size();
    if (blocks[index].valid && blocks[index].tag == addr) {
        if (blocks[index].state == MESIState::Modified || blocks[index].state == MESIState::Exclusive) {
            blocks[index].state = MESIState::Shared;
            // Adicionalmente, se debe manejar la escritura de vuelta a la memoria si está Modificado
        }
    }
}

void Cache::snoopBusRdX(int addr) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    int index = addr % blocks.size();
    if (blocks[index].valid && blocks[index].tag == addr) {
        blocks[index].valid = false;
        blocks[index].state = MESIState::Invalid;
        // Adicionalmente, se debe manejar la escritura de vuelta a la memoria si está Modificado
    }
}

void Cache::snoopBusUpgr(int addr) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    int index = addr % blocks.size();
    if (blocks[index].valid && blocks[index].tag == addr && blocks[index].state == MESIState::Shared) {
        blocks[index].valid = false;
        blocks[index].state = MESIState::Invalid;
    }
}

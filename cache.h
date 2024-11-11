#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <mutex>

enum class MESIState { Modified, Exclusive, Shared, Invalid };

// Estructura para representar una línea de caché
struct CacheBlock {
    uint64_t data;
    bool valid;
    int tag;

    MESIState state;

    CacheBlock() : data(0), valid(false), tag(-1), state(MESIState::Invalid) {}
};

class Cache {
public:
    Cache(int numBlocks);

    uint64_t load(int addr);
    void store(int addr, uint64_t value);
    void processBusTransaction(int addr, MESIState transactionType); // Placeholder para interacción MESI

    void snoopBusRd(int addr);
    void snoopBusRdX(int addr);
    void snoopBusUpgr(int addr);

private:
    std::vector<CacheBlock> blocks;
    std::mutex cache_mutex; // Mutex para sincronizar el acceso a la caché
};

#endif // CACHE_H

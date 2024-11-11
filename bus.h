#ifndef BUS_H
#define BUS_H

#include <vector>
#include <mutex>
#include "cache.h"

class Bus {
public:
    Bus();
    void registerCache(Cache* cache);
    void sendBusRd(int addr);
    void sendBusRdX(int addr);
    void sendBusUpgr(int addr);

private:
    std::vector<Cache*> caches;
    std::mutex bus_mutex;
};

#endif // BUS_H

#include "bus.h"
#include <iostream>

Bus::Bus() {}

void Bus::registerCache(Cache* cache) {
    caches.push_back(cache);
}

void Bus::sendBusRd(int addr) {
    std::lock_guard<std::mutex> lock(bus_mutex);
    for (auto cache : caches) {
        cache->snoopBusRd(addr);
    }
}

void Bus::sendBusRdX(int addr) {
    std::lock_guard<std::mutex> lock(bus_mutex);
    for (auto cache : caches) {
        cache->snoopBusRdX(addr);
    }
}

void Bus::sendBusUpgr(int addr) {
    std::lock_guard<std::mutex> lock(bus_mutex);
    for (auto cache : caches) {
        cache->snoopBusUpgr(addr);
    }
}

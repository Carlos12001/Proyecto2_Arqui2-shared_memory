// bus.cpp
#include "bus.h"

Bus::Bus() {}

void Bus::registerCache(Cache *cache) { caches.push_back(cache); }

void Bus::clearCaches() { caches.clear(); }

void Bus::sendBusRd(int addr, Cache *requester) {
  std::lock_guard<std::mutex> lock(bus_mutex);
  for (auto cache : caches) {
    if (cache != requester) {
      cache->snoopBusRd(addr);
    }
  }
}

void Bus::sendBusRdX(int addr, Cache *requester) {
  std::lock_guard<std::mutex> lock(bus_mutex);
  for (auto cache : caches) {
    if (cache != requester) {
      cache->snoopBusRdX(addr);
    }
  }
}

void Bus::sendBusUpgr(int addr, Cache *requester) {
  std::lock_guard<std::mutex> lock(bus_mutex);
  for (auto cache : caches) {
    if (cache != requester) {
      cache->snoopBusUpgr(addr);
    }
  }
}

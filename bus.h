// bus.h
#ifndef BUS_H
#define BUS_H

#include <mutex>
#include <vector>

class Cache;  // Declaración anticipada

class Bus {
 public:
  Bus();
  void registerCache(Cache *cache);

  void sendBusRd(int addr, Cache *requester);
  void sendBusRdX(int addr, Cache *requester);
  void sendBusUpgr(int addr, Cache *requester);

 private:
  std::vector<Cache *> caches;
  std::mutex bus_mutex;
};

#endif  // BUS_H

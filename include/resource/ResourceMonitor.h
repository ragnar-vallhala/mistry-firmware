#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H
#include <cstdint>
class ResourceMonitor{
public:
  static uint8_t get_cpu_usage();
};
#endif // !RESOURCE_MONITOR_H

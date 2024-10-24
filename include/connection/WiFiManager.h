#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include "connection/ConnectionManager.h"
#include <string>
#include "FreeRTOS.h"
#include "lwip/netif.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "portmacro.h"
#include "task.h"
#include <cstdint>
#include <cyw43_country.h>
#include "Config.h"
class WiFiManager : public ConnectionManager {
public:
  ~WiFiManager();
  static WiFiManager* get_wifi_manager();
  bool establish(std::string *msg = nullptr);
  bool is_connected_wifi();
  void send(std::string msg);
  void recieve(std::string *msg);
  void recieve_file(std::string *content,
                    std::string *fileName); // TODO: Implement later
private:
  WiFiManager();
  static WiFiManager* obj;
  static bool isConnectedWiFi;
};
#endif // !WIFI_MANAGER_H

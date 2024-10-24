#include "connection/WiFiManager.h"
#include <cyw43.h>
#include <stdio.h>
#include <string.h>

WiFiManager* WiFiManager::obj = nullptr;
bool WiFiManager::isConnectedWiFi = false;
WiFiManager::~WiFiManager() {
    if (obj)
        delete obj;
}

// Constructor
WiFiManager::WiFiManager() {
    cyw43_arch_init_with_country(CYW43_COUNTRY_INDIA);
    cyw43_arch_enable_sta_mode();
    netif_set_hostname(cyw43_state.netif, HOSTNAME);
}
bool WiFiManager::is_connected_wifi()
{
  return isConnectedWiFi;
}
WiFiManager* WiFiManager::get_wifi_manager() {
    if (obj == nullptr)
        obj = new WiFiManager();
    return obj;
}

int scan_result_callback(void *env, const cyw43_ev_scan_result_t *result) {
    if (result == nullptr) {
        return 0; // No result
    }

    if (strncmp((const char *)result->ssid, WIFI_SSID, sizeof(result->ssid)) == 0) {
        return 1; // Found the network
    }
    return 0; // Continue scanning
}
bool WiFiManager::establish(std::string *msg) {
if (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_JOIN) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        printf("Already connected to Wi-Fi.\n");
    isConnectedWiFi = true;
        return true;
    }

    printf("Not connected. Scanning for available networks...\n");

    // Scan options setup
    cyw43_wifi_scan_options_t scan_options = {};

    // Start a scan for available Wi-Fi networks
    int scan_result = cyw43_wifi_scan(&cyw43_state, &scan_options, nullptr, scan_result_callback);
    if (scan_result < 0) {
        printf("Wi-Fi scan failed with error: %d\n", scan_result);
        if (msg) {
            *msg = "Wi-Fi scan failed with error code: " + std::to_string(scan_result);
        }
    isConnectedWiFi = false;
        return false;
    }

    // Wait for the scan results (you might implement a better wait mechanism)
    vTaskDelay(pdMS_TO_TICKS(6000));

    // Attempt to connect to Wi-Fi
    int connect_result = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD,
                                                            CYW43_AUTH_WPA2_AES_PSK, 10000);

    // Provide detailed feedback on the connection attempt
    if (connect_result == 0) {
        printf("Wi-Fi connected successfully!\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    isConnectedWiFi = true;
        return true;
    } else {
        // Provide error feedback if connection fails
        printf("Failed to connect to Wi-Fi, error code: %d\n", connect_result);
        if (msg) {
            *msg = "Wi-Fi connection failed, error code: " + std::to_string(connect_result);
        }
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    isConnectedWiFi = false;
        return false;
    }
}
// Empty method to send data (to be implemented)
void WiFiManager::send(std::string msg) {}

// Empty method to receive data (to be implemented)
void WiFiManager::recieve(std::string* msg) {}

// Empty method to receive a file (to be implemented)
void WiFiManager::recieve_file(std::string* content, std::string* fileName) {}


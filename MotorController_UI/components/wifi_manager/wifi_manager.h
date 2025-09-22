#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include "esp_wifi.h"

void wifi_manager_init();
void wifi_manager_scan();
void wifi_manager_connect(const char* ssid, const char* password);
const char* wifi_reason_to_string(uint8_t reason);
const char* wifi_mgr_get_connected_ssid();
#endif // WIFI_MANAGER_H
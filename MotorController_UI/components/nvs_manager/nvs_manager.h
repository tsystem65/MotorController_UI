#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

typedef struct {
    char ssid[32];
    char password[64];
} wifi_creds_t;

esp_err_t nvs_manager_init(void);

esp_err_t nvs_manager_set_machine_last_state(double cable_length_counter, double used_cable_length);
esp_err_t nvs_manager_get_machine_last_state(double* cable_length_counter, double* used_cable_length);
esp_err_t nvs_manager_reset_machine_last_state(void);

esp_err_t nvs_manager_set_wifi_creds(const wifi_creds_t* creds);
esp_err_t nvs_manager_get_wifi_creds(wifi_creds_t* creds);

#endif // NVS_MANAGER_H
#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include "esp_err.h"

/**
 * @brief Perform OTA firmware update over HTTPS.
 *
 * Downloads firmware from OTA_URL, writes it to the OTA partition,
 * reports progress via event manager, and finalizes the update.
 *
 * @return
 *      - ESP_OK: OTA update completed successfully
 *      - ESP_FAIL or error code: OTA update failed
 */
void do_https_ota(void *args);

#endif // OTA_MANAGER_H
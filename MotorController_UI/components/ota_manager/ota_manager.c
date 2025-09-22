#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_https_ota.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "event_manager.h"
#include "wifi_manager.h"

#define OTA_URL "https://tsmr65firmware.blob.core.windows.net/tsm-r65-ota-firmware/MotorController_UI.bin?sp=r&st=2025-09-03T13:15:42Z&se=2029-09-03T21:30:42Z&spr=https&sv=2024-11-04&sr=b&sig=RhoJj1mejjYrrSPmd%2FhVDiEMSVc6IwECXk1FlVSx6Mk%3D"

static const char *TAG = "HTTPS_OTA";

void do_https_ota(void *args)
{
    ESP_LOGI(TAG, "🚀 Starting OTA from Azure Blob...");
    
    while (wifi_mgr_get_connected_ssid() == NULL)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    esp_http_client_config_t http_cfg = {
        .url = OTA_URL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 30000,
        .keep_alive_enable = true,
        .buffer_size = 8192,
        .buffer_size_tx = 1024,
    };

    esp_https_ota_config_t ota_cfg = {
        .http_config = &http_cfg,
        .partial_http_download = true,
        .max_http_request_size = 8192,
    };

    esp_https_ota_handle_t handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_cfg, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "❌ OTA Begin failed: %s", esp_err_to_name(err));
        event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_BEGIN_FAILED, NULL, 0);
        return;
    }

    ESP_LOGI(TAG, "📡 OTA session started");
    event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_STARTED, NULL, 0);

    int total = esp_https_ota_get_image_size(handle);
    if (total > 0) {
        ESP_LOGI(TAG, "📦 OTA image size: %d bytes", total);
    } else {
        ESP_LOGI(TAG, "📦 OTA image size: unknown (chunked transfer)");
    }

    int last_logged_progress = -1;
    uint32_t start_time = esp_log_timestamp();

    while (1) {
        err = esp_https_ota_perform(handle);
        if (err == ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            int done = esp_https_ota_get_image_len_read(handle);

            if (total > 0) {
                int progress = (100 * done) / total;
                uint32_t current_time = esp_log_timestamp();

                if ((progress / 2) != (last_logged_progress / 2) ||
                    (current_time - start_time) >= 10000) {
                    ESP_LOGI(TAG, "📥 Progress: %d%% (%d/%d bytes)", progress, done, total);
                    ota_mgr_perform_event_data_t perform_data = {0};
                    perform_data.total_image_size_bytes = total;
                    perform_data.bytes_downloaded = done;
                    
                    event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_PERFORM, &perform_data, sizeof(perform_data));
                    last_logged_progress = progress;
                    start_time = current_time;
                }
            } else {
                ESP_LOGI(TAG, "📥 Downloaded: %d bytes", done);
            }
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "❌ OTA perform failed: %s", esp_err_to_name(err));
            event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_PERFORM_FAILED, NULL, 0);
            esp_https_ota_abort(handle);
            return;
        }
        break;
    }

    if (!esp_https_ota_is_complete_data_received(handle)) {
        ESP_LOGE(TAG, "❌ Complete data not received");
        event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_COMPLETE_DATA_NOT_RECEIVED, NULL, 0);
        esp_https_ota_abort(handle);
        return;
    }

    err = esp_https_ota_finish(handle);
    if (err != ESP_OK) {
        event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_FINISH_FAILED, NULL, 0);
        ESP_LOGE(TAG, "❌ OTA finish failed: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "🎉 OTA update completed successfully!");
    event_manager_notify(OTA_MGR_EVENTS, OTA_MGR_EVENT_COMPLETED, NULL, 0);
    esp_restart();
}
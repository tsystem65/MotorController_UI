#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "driver/uart.h"
#include "wifi_manager.h"
#include "nvs_manager.h"
#include "event_manager.h"
#include "uart_manager.h"
#include "ota_manager.h"

// Driver
#include "driver/i2c.h"
#include "esp_lcd_touch_gt911.h"

// Including the UI
#include "ui/ui.h"
// Importing the file
#include "../components/espressif__esp_lcd_touch/display.h"

#define FW_VERSION_MAJOR 0
#define FW_VERSION_MINOR 8
#define FW_VERSION_PATCH 1

static const char *INFO_TAG = "INFO";

// Функції для обробки подій LVGL
void StartMotors(lv_event_t *e) {
    ESP_LOGI(INFO_TAG, "Starting Motors...");
    //uart_send_command("Start");
}

void StopMotors(lv_event_t *e) {
    ESP_LOGI(INFO_TAG, "Decelerating Motors...");
    //uart_send_command("Stop");
}

void ImmediateStop(lv_event_t *e) {
    ESP_LOGI(INFO_TAG, "Immediate Stop Motors...");
    //uart_send_command("ImmediateStop");
}

// Головна функція
void app_main(void) {
    ESP_LOGI(TAG, "Starting app_main...");
    ESP_LOGI(TAG, "Calling display...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    event_manager_init();
    nvs_manager_init();
    // wifi_manager_init();
    // wifi_manager_connect("POCO F5", "20042005");
    // vTaskDelay(pdMS_TO_TICKS(10000));

    display();

    esp_lcd_rgb_panel_restart(panel_handle);

    vTaskDelay(pdMS_TO_TICKS(2000));

    char fw_version_json_data[128] = {0};
    sniprintf(fw_version_json_data, sizeof(fw_version_json_data), "{\"command\": \"fw_version_check\", \"fw_version\": \"%d.%d.%d\"}", FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);
    uart_send_command(fw_version_json_data);

    ESP_LOGI(TAG, "app_main completed");
}
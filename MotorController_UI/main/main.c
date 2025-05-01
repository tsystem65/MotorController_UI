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
#include "uart_manager.h"

// Driver
#include "driver/i2c.h"
#include "esp_lcd_touch_gt911.h"

// Including the UI
#include "ui/ui.h"
// Importing the file
#include "../components/espressif__esp_lcd_touch/display.h"

#define TAG "UART_TRANSMITTER"
#define UART_NUM UART_NUM_2
#define UART_TX_PIN 43  // Використовуємо стандартні піни USB-to-UART
#define UART_RX_PIN 44
#define UART_BAUD_RATE 115200
#define UART_BUFFER_SIZE (1024)

static const char *INFO_TAG = "INFO";

// Функції для обробки подій LVGL
void StartMotors(lv_event_t *e) {
    ESP_LOGI(INFO_TAG, "Starting Motors...");
    uart_send_command("Start");
}

void StopMotors(lv_event_t *e) {
    ESP_LOGI(INFO_TAG, "Decelerating Motors...");
    uart_send_command("Stop");
}

void ImmediateStop(lv_event_t *e) {
    ESP_LOGI(INFO_TAG, "Immediate Stop Motors...");
    uart_send_command("ImmediateStop");
}

// Головна функція
void app_main(void) {
    ESP_LOGI(TAG, "Starting app_main...");
    //uart_init();
    ESP_LOGI(TAG, "Calling display...");
    display();

    // Створюємо задачу для прийому повідомлень
    ESP_LOGI(TAG, "Creating uart_rx_task...");
    ESP_LOGI(TAG, "app_main completed");
}
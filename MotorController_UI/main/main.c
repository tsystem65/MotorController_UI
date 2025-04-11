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

// Ініціалізація UART
// static void uart_init(void) {
//     ESP_LOGI(TAG, "Initializing UART on GPIO %d (TX) and %d (RX)...", UART_TX_PIN, UART_RX_PIN);
//     uart_config_t uart_config = {
//         .baud_rate = UART_BAUD_RATE,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .source_clk = UART_SCLK_DEFAULT,
//     };
//     ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
//     ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
//     ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, 0));
//     ESP_LOGI(TAG, "UART initialized successfully");
// }

// // Відправка команди через UART
// static void send_command(const char *command) {
//     uart_write_bytes(UART_NUM, command, strlen(command));
//     uart_write_bytes(UART_NUM, "\n", 1);  // Додаємо новий рядок як роздільник
//     ESP_LOGI(TAG, "Sent command: %s", command);
// }

// Задача для прийому повідомлень через UART
static void uart_rx_task(void *arg) {
    uint8_t data[UART_BUFFER_SIZE];  // Буфер для прийому байтів
    char text_buffer[UART_BUFFER_SIZE];  // Тимчасовий буфер для тексту

    while (1) {
        int length = uart_read_bytes(UART_NUM, data, UART_BUFFER_SIZE - 1, 100 / portTICK_PERIOD_MS);
        if (length > 0) {
            data[length] = '\0';  // Додаємо нульовий термінатор
            strncpy(text_buffer, (char *)data, length);
            text_buffer[length] = '\0';
            ESP_LOGI(TAG, "Received: %s", text_buffer);

            // Виводимо отримане повідомлення на дисплей через LVGL
            if (ui_StartButtonLabel) {
                ESP_LOGI(TAG, "Updating label with: %s", text_buffer);
                lv_label_set_text(ui_StartButtonLabel, text_buffer);
            } else {
                ESP_LOGE(TAG, "ui_StartButtonLabel is NULL!");
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

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
    //xTaskCreate(uart_rx_task, "uart_rx_task", 4096, NULL, 10, NULL);
    ESP_LOGI(TAG, "app_main completed");
}
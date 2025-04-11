#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "uart_manager.h"
//#include "ui.h"
#include <string.h>

#define TAG "UART_TRANSMITTER"
#define UART_NUM UART_NUM_2
#define UART_TX_PIN 43  // Використовуємо стандартні піни USB-to-UART
#define UART_RX_PIN 44
#define UART_BAUD_RATE 115200
#define UART_BUFFER_SIZE (1024)
#define UART_TASK_STACK_SIZE 4096

static QueueHandle_t uart_queue = NULL;
static void (*data_callback)(const char*) = NULL;

static void uart_event_task(void *arg) {
    uart_event_t event;
    uint8_t data[UART_BUFFER_SIZE];
    
    while (1) {
        if (xQueueReceive(uart_queue, &event, 100 / portTICK_PERIOD_MS)) {
            switch (event.type) {
                case UART_DATA:
                    ESP_LOGI(TAG, "UART_DATA event, size: %d", event.size);
                    int len = uart_read_bytes(UART_NUM, data, event.size, pdMS_TO_TICKS(100));
                    if (len > 0) {
                        data[len] = '\0';
                        if (data_callback) {
                            data_callback((const char*)data);
                        }
                        ESP_LOGI(TAG, "Received: %.*s", len, data);
                    } else {
                        ESP_LOGW(TAG, "No data read, len: %d", len);
                    }
                    break;
                case UART_FIFO_OVF:
                    ESP_LOGE(TAG, "FIFO Overflow");
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;
                    
                case UART_BUFFER_FULL:
                    ESP_LOGE(TAG, "RX Buffer Full - data may be lost");
                    uart_flush_input(UART_NUM);  // Очищаємо буфер
                    break;

                case UART_BREAK:
                    ESP_LOGW(TAG, "UART Break detected");
                    break;
                default:
                    ESP_LOGI(TAG, "Unhandled event type: %d", event.type);
                    break;
            }
        } else {
            ESP_LOGD(TAG, "No event in queue after timeout");
        }
        ESP_LOGD(TAG, "Stack remaining: %u bytes", uxTaskGetStackHighWaterMark(NULL));
    }
    vTaskDelete(NULL);
}

// Ініціалізація UART
void uart_init(void) {
    ESP_LOGI(TAG, "Initializing UART on GPIO %d (TX) and %d (RX)...", UART_TX_PIN, UART_RX_PIN);
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 20, &uart_queue, 0));

    xTaskCreate(uart_event_task, "uart_event_task", UART_TASK_STACK_SIZE, NULL, 10, NULL);

    ESP_LOGI(TAG, "UART initialized successfully");
}

void uart_send_command(const char *command) {
    uart_write_bytes(UART_NUM, command, strlen(command));
    uart_write_bytes(UART_NUM, "\n", 1);  // Додаємо новий рядок як роздільник
    ESP_LOGI(TAG, "Sent command: %s", command);
}

bool uart_register_data_callback(void (*callback)(const char*)) {
    if (callback == NULL) return false;
    data_callback = callback;
    return true;
}
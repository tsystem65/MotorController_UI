#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"
#include "uart_manager.h"
#include <string.h>

#define TAG "UART_TRANSMITTER"
#define UART_NUM UART_NUM_2
#define UART_TX_PIN 43  // Використовуємо стандартні піни USB-to-UART
#define UART_RX_PIN 44
#define UART_BAUD_RATE 115200
#define UART_BUFFER_SIZE 2048
#define UART_TASK_STACK_SIZE 8192

#define PATTERN_CHAR    '\n'
#define PATTERN_LEN     1
#define PATTERN_IDLE_TICKS 10000   // таймаут перед викидом невикористаних байтів
#define PATTERN_QLEN    10

static QueueHandle_t uart_queue = NULL;
static void (*data_callback)(const char*) = NULL;

static void uart_event_task(void *arg) {
    uart_event_t event;
    static uint8_t data[UART_BUFFER_SIZE + 1];

    while (1) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {
            switch (event.type) {
                case UART_DATA:
                    // За потреби можете ігнорувати, або логувати як у початковій версії
                    ESP_LOGD(TAG, "UART_DATA event, size: %d", event.size);
                    break;

                case UART_PATTERN_DET: {
                    int pos = uart_pattern_pop_pos(UART_NUM);
                    if (pos < 0) {
                        ESP_LOGW(TAG, "Pattern DET but no pos");
                        break;
                    }
                    // Читаємо весь рядок разом із '\n'
                    int len = uart_read_bytes(UART_NUM, data, pos + 1, pdMS_TO_TICKS(100));
                    if (len > 0) {
                        data[len - 1] = '\0';    // замінюємо '\n' на 0
                        ESP_LOGI(TAG, "Received full JSON: %s", (char*)data);
                        if (data_callback) {
                            data_callback((const char*)data);
                        }
                    } else {
                        ESP_LOGW(TAG, "Pattern DET but read len=%d", len);
                    }
                    break;
                }

                case UART_FIFO_OVF:
                    ESP_LOGE(TAG, "FIFO Overflow");
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;

                case UART_BUFFER_FULL:
                    ESP_LOGE(TAG, "RX Buffer Full");
                    uart_flush_input(UART_NUM);
                    xQueueReset(uart_queue);
                    break;

                case UART_BREAK:
                    ESP_LOGW(TAG, "UART Break detected");
                    break;

                default:
                    ESP_LOGI(TAG, "Unhandled event type: %d", event.type);
                    break;
            }
        }
        ESP_LOGD(TAG, "Stack remaining: %u bytes", uxTaskGetStackHighWaterMark(NULL));
    }
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
    //ESP_ERROR_CHECK(uart_driver_install(UART_NUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 20, &uart_queue, 0));

    uart_driver_install(UART_NUM, UART_BUFFER_SIZE * 2, 0, PATTERN_QLEN, &uart_queue, 0);
    ESP_ERROR_CHECK(uart_enable_pattern_det_baud_intr(
        UART_NUM,
        PATTERN_CHAR,        // символ, що шукаємо
        PATTERN_LEN,         // довжина шаблону (кількість повторів)
        PATTERN_IDLE_TICKS,  // такти затримки перед детекцією
        PATTERN_QLEN,        // поріг заповнення буфера перед IRQ
        0                    // прапори (не використовується)
    ));
    
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
// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/uart.h"

// #define UART_NUM UART_NUM_2
// #define TXD_PIN 43
// #define RXD_PIN 44

// void uart_init(void) {
//     uart_config_t uart_config = {
//         .baud_rate = 115200,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//         .source_clk = UART_SCLK_DEFAULT,
//     };

//     esp_err_t ret = uart_param_config(UART_NUM, &uart_config);
//     if (ret != ESP_OK) {
//         printf("UART config failed: %d\n", ret);
//     }

//     ret = uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
//     if (ret != ESP_OK) {
//         printf("UART set pin failed: %d\n", ret);
//     }

//     ret = uart_driver_install(UART_NUM, 256, 0, 0, NULL, 0);
//     if (ret != ESP_OK) {
//         printf("UART driver install failed: %d\n", ret);
//     }
// }

// void app_main(void) {
//     uart_init();
//     const char *message = "HELLOOOOOOOOOOOO!!!!!!\n";

//     // Вимикаємо вивід логу через UART2, якщо він там
//     uart_driver_delete(UART_NUM_0); // Видаляємо UART0, якщо він був
//     uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0); // Ініціалізуємо UART0 для printf

//     while (1) {
//         int bytes_written = uart_write_bytes(UART_NUM, message, strlen(message));
//         if (bytes_written > 0) {
//             printf("Sent %d bytes via UART2\n", bytes_written); // Виводимо через UART0
//         } else {
//             printf("Failed to send via UART2: %d\n", bytes_written);
//         }
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }
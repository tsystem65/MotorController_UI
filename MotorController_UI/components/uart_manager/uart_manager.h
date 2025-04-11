#ifndef UART_MANAGER_H
#define UART_MANAGER_H

void uart_init(void);
void uart_send_command(const char *command);
bool uart_register_data_callback(void (*callback)(const char* data));

#endif
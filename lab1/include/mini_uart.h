#ifndef _MINI_UART_H
#define _MINI_UART_H

void uart_init();
void uart_send_string(char *str);
void uart_send(char c);
char uart_recv();
#endif
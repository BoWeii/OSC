#include "allocator.h"
#include "mini_uart.h"

void* malloc(size_t size){
    uart_send_string("in malloc\n");
}
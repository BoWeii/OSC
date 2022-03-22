#include "mini_uart.h"
#include "dtb.h"
#include "utils_s.h"
#include "shell.h"
#include "timer.h"

extern void *_dtb_ptr;

void kernel_main(void)
{
    // uart_init();
    uart_send_string("Hello, world!\n");
    fdt_traverse(get_initramfs_addr,_dtb_ptr);
    uart_send_string("kernel Exception level: ");
    uart_hex(get_el());
    uart_send('\n');
    core_timer_enable();
    shell();
}
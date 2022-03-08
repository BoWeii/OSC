#include "mini_uart.h"
#include "utils_c.h"
#include "utils_assembly.h"
#include "shell.h"
#define BUFFER_MAX_SIZE 256u

void load_img()
{
    char *const kernel_addr = (char *)0x80000;
    uart_send_string("Please sent the kernel image size:");
    char buffer[BUFFER_MAX_SIZE];
    read_command(buffer);
    unsigned int img_size = utils_str2uint_dec(buffer);
    uart_send_string("Start to load the kernel image... \n");

    unsigned char *current = kernel_addr;
    while (img_size--)
    {
        *current = uart_recv();
        current++;
        uart_send('.');
    }
    uart_send_string("loading...\n");
    branchAddr(kernel_addr);
}

void bootloader_main(void)
{
    uart_init();
    uart_send_string("In bootloader_main!\n");
    load_img();
}
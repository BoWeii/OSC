#include "shell.h"
#include "mini_uart.h"
#include "utils_c.h"
#include "utils_assembly.h"
#include "peripheral/mailbox.h"
#include "initrd.h"
#include "allocator.h"
#include <stddef.h>
#define BUFFER_MAX_SIZE 256u
#define COMMNAD_LENGTH_MAX 20u

static const char *command_list[] = {"help", "hello", "reboot", "info"};
static const char *command_explain[] = {"print this help menu\n", "print Hello World!\n", "reboot the device\n", "the mailbox hardware info\n"};

void read_command(char *buffer)
{
    size_t index = 0;
    while (1)
    {
        buffer[index] = uart_recv();
        uart_send(buffer[index]);
        if (buffer[index] == '\n')
        {
            break;
        }
        index++;
    }
    buffer[index + 1] = '\0';
    utils_newline2end(buffer);
    uart_send('\r');
}

void help()
{
    uart_send_string("help\t\t: ");
    uart_send_string("print this help menu\n");
    uart_send_string("hello\t\t: ");
    uart_send_string("print Hello World!\n");
    uart_send_string("reboot\t\t: ");
    uart_send_string("reboot the device\n");
    uart_send_string("info\t\t: ");
    uart_send_string("the mailbox hardware info\n");
    uart_send_string("load_img\t: ");
    uart_send_string("using the loadImg.sh to load kernel image\n");
    uart_send_string("ls\t\t: ");
    uart_send_string("list the all file\n");
    uart_send_string("cat\t\t: ");
    uart_send_string("print the file content\n");
    uart_send_string("malloc\t\t: ");
    uart_send_string("a simple memory allocator\n");

    // for (size_t i = 0; i < sizeof(command_list) / sizeof(const char *); i++)
    // {
    //     uart_hex(i);
    //     uart_send_string(command_list[i]);
    //     // int command_len = 0;
    //     // while (command_list[i][command_len] != '\0')
    //     // {
    //     //     command_len++;
    //     // }
    //     // for (int k = COMMNAD_LENGTH_MAX - command_len; k >= 0; k--)
    //     // {
    //     //     uart_send(' ');
    //     // }

    //     uart_send_string(":");
    //     uart_send_string(command_explain[i]);
    // }
}

void hello()
{
    uart_send_string("Hello World!\n");
}

void info()
{
    get_board_revision();
    get_arm_memory();
}

void load_img()
{
    char *const kernel_addr = (char *)0x40000;
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

void parse_command(char *buffer)
{

    if (buffer[0] == '\0')
    { // enter empty
        return;
    }
    else if (utils_str_compare(buffer, "help") == 0)
    {
        help();
    }
    else if (utils_str_compare(buffer, "hello") == 0)
    {
        hello();
    }
    else if (utils_str_compare(buffer, "reboot") == 0)
    {
        uart_send_string("rebooting...\n");
        reset(1000);
    }
    else if (utils_str_compare(buffer, "info") == 0)
    {
        info();
    }
    else if (utils_str_compare(buffer, "load_img") == 0)
    {
        load_img();
    }
    else if (utils_str_compare(buffer, "ls") == 0)
    {
        initrd_ls();
    }
    else if (utils_str_compare(buffer, "cat") == 0)
    {
        uart_send_string("Filename: ");
        char buffer[BUFFER_MAX_SIZE];
        read_command(buffer);
        initrd_cat(buffer);
    }
    else if (utils_str_compare(buffer, "malloc") == 0)
    {
        char *a = malloc(sizeof("1234"));
        char *b = malloc(sizeof("789"));
        a[0] = '0';
        a[1] = '1';
        a[2] = '2';
        a[3] = '3';
        a[4] = '\0';
        b[0] = '7';
        b[1] = '8';
        b[2] = '9';
        b[3] = '\0';
        // uart_hex((unsigned int)a);
        // uart_send('\n');
        uart_send_string(a);
        uart_send('\n');
        // uart_hex((unsigned int)b);
        // uart_send('\n');
        uart_send_string(b);
        uart_send('\n');
    }
    else
    {
        uart_send_string("commnad '");
        uart_send_string(buffer);
        uart_send_string("' not found\n");
    }
}

void shell()
{
    while (1)
    {
        char buffer[BUFFER_MAX_SIZE];
        uart_send_string("$ ");
        read_command(buffer);
        parse_command(buffer);
    }
}
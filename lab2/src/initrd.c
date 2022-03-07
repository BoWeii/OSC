#include "initrd.h"
#include "utils_c.h"
#include "mini_uart.h"

unsigned long hex2dec(char *s)
{
    unsigned long r = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (s[i] >= '0' && s[i] <= '9')
        {
            r = r * 16 + s[i] - '0';
        }
        else
        {
            r = r * 16 + s[i] - 'a' + 10;
        }
    }
    return r;
}

void align_4(void *size)
{
    unsigned long *x = (unsigned long *)size;
    if ((*x) & 3)
    {
        (*x) += 4 - ((*x) & 3);
    }
}

void initrd_list()
{
    /*
     cpio archive comprises a header record with basic numeric metadata followed by
     the full pathname of the entry and the file data.
    */

    // char *addr = (char *)0x8000000; // qemu
    char *addr = (char *)0x20000000; // raspi3

    while (utils_str_compare((char *)(addr + sizeof(cpio_header)), "TRAILER!!!") != 0)
    {
        cpio_header *header = (cpio_header *)addr;

        unsigned long pathname_size = hex2dec(header->c_namesize);
        unsigned long file_size = hex2dec(header->c_filesize);
        unsigned long headerPathname_size = sizeof(cpio_header) + pathname_size;

        align_4(&headerPathname_size); // The pathname is followed by NUL bytes so that the total size of the fixed header plus pathname is a multiple	of four.
        align_4(&file_size);           // Likewise, the	file data is padded to a multiple of four bytes.

        uart_send_string(addr + sizeof(cpio_header));
        uart_send_string("\r\n");

        addr += (headerPathname_size + file_size);
    }
}

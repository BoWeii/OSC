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

/*
    The pathname is followed by NUL bytes so that the total size of the fixed header plus pathname is a multiple of 4.
    Likewise, the file data is padded to a multiple of 4 bytes.
*/
void align_4(void *size) // aligned to 4 byte
{
    unsigned long *x = (unsigned long *)size;
    if ((*x) & 3)
    {
        (*x) += 4 - ((*x) & 3);
    }
}

char *findFile(char *name)
{
    char *addr = CPIO_ADDR;
    while (utils_str_compare((char *)(addr + sizeof(cpio_header)), "TRAILER!!!") != 0)
    {
        if ((utils_str_compare((char *)(addr + sizeof(cpio_header)), name) == 0))
        {
            return addr;
        }
        cpio_header *header = (cpio_header *)addr;
        unsigned long pathname_size = hex2dec(header->c_namesize);
        unsigned long file_size = hex2dec(header->c_filesize);
        unsigned long headerPathname_size = sizeof(cpio_header) + pathname_size;

        align_4(&headerPathname_size); 
        align_4(&file_size);           
        addr += (headerPathname_size + file_size);
    }
    return 0;
}
void initrd_ls()
{
    char *addr = CPIO_ADDR;
    while (utils_str_compare((char *)(addr + sizeof(cpio_header)), "TRAILER!!!") != 0)
    {
        cpio_header *header = (cpio_header *)addr;
        unsigned long pathname_size = hex2dec(header->c_namesize);
        unsigned long file_size = hex2dec(header->c_filesize);
        unsigned long headerPathname_size = sizeof(cpio_header) + pathname_size;

        align_4(&headerPathname_size); 
        align_4(&file_size);           

        uart_send_string(addr + sizeof(cpio_header)); // print the file name
        uart_send_string("\n");

        addr += (headerPathname_size + file_size);
    }
}

void initrd_cat(char *filename)
{
    char *target = findFile(filename);
    if (target)
    {
        cpio_header *header = (cpio_header *)target;
        unsigned long pathname_size = hex2dec(header->c_namesize);
        unsigned long file_size = hex2dec(header->c_filesize);
        unsigned long headerPathname_size = sizeof(cpio_header) + pathname_size;

        align_4(&headerPathname_size); 
        align_4(&file_size);           

        char *file_content = target + headerPathname_size;
        for (unsigned int i = 0; i < file_size; i++)
        {
            uart_send(file_content[i]); // print the file content
        }
        uart_send_string("\n");
    }
    else
    {
        uart_send_string("Not found the file\n");
    }
}

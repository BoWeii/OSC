#include <stddef.h>


#define SYS_GETPID 0
#define SYS_UART_RECV 1
#define SYS_UART_WRITE 2
#define SYS_EXEC 3
#define SYS_FORK 4
#define SYS_EXIT 5
#define SYS_MBOX_CALL 6
#define SYS_KILL_PID 7

int start(void) __attribute__((section(".start")));

unsigned long syscall(unsigned long syscall_num,
               void *x0,
               void *x1,
               void *x2,
               void *x3,
               void *x4,
               void *x5)
{
    unsigned long result;

    asm volatile(
        "ldr x8, %0\n"
        "ldr x0, %1\n"
        "ldr x1, %2\n"
        "ldr x2, %3\n"
        "ldr x3, %4\n"
        "ldr x4, %5\n"
        "ldr x5, %6\n"
        "svc 0\n" ::"m"(syscall_num),
        "m"(x0), "m"(x1),
        "m"(x2), "m"(x3), "m"(x4), "m"(x5));

    asm volatile(
        "str x0, %0\n"
        : "=m"(result));

    return result;
}
/* system call */
int getpid()
{
    return (int)syscall(SYS_GETPID, 0, 0, 0, 0, 0, 0);
}

void uart_recv(const char buf[], size_t size)
{
    syscall(SYS_UART_RECV, (void *)buf, (void *)size, 0, 0, 0, 0);
}

void uart_write(const char buf[], size_t size)
{
    syscall(SYS_UART_WRITE, (void *)buf, (void *)size, 0, 0, 0, 0);
}


/* normal function */

static void uart_send(char c)
{
    uart_write(&c, 1);
}

void uart_send_string(const char *str)
{
    while (*str)
    {
        uart_send(*str++);
    }
}
void uart_hex(unsigned int d)
{
    unsigned int n;
    int c;
    uart_send_string("0x");
    for (c = 28; c >= 0; c -= 4)
    {
        n = (d >> c) & 0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n += n > 9 ? 0x57 : 0x30;
        uart_send(n);
    }
}
void uart_dec(unsigned int num)
{
    if (num == 0)
        uart_send('0');
    else
    {
        if (num >= 10)
            uart_dec(num / 10);
        uart_send(num % 10 + '0');
    }
}



int start(void)
{
    // char buf1[0x10] = {0};
    int pid;

    pid = getpid();
    uart_hex(pid);
    while(1);
    return 0;
}

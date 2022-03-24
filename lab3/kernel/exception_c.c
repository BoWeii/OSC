#include "utils_s.h"
#include "mini_uart.h"
#include "peripheral/mini_uart.h"
#include "exception_c.h"
#define AUX_IRQ (1 << 29)

void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }
void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void default_handler()
{
    disable_interrupt();
    unsigned long spsr = read_sysreg(spsr_el1);
    unsigned long elr = read_sysreg(elr_el1);
    unsigned long esr = read_sysreg(esr_el1);
    uart_send_string("spsr_el1: ");
    uart_hex(spsr);
    uart_send('\n');
    uart_send_string("elr_el1: ");
    uart_hex(elr);
    uart_send('\n');
    uart_send_string("esr_el1: ");
    uart_hex(esr);
    uart_send_string("\n\n");
    enable_interrupt();
}


void lower_sync_handler()
{
    disable_interrupt();
    unsigned long spsr = read_sysreg(spsr_el1);
    unsigned long elr = read_sysreg(elr_el1);
    unsigned long esr = read_sysreg(esr_el1);
    uart_send_string("spsr_el1: ");
    uart_hex(spsr);
    uart_send('\n');
    uart_send_string("elr_el1: ");
    uart_hex(elr);
    uart_send('\n');
    uart_send_string("esr_el1: ");
    uart_hex(esr);
    uart_send_string("\n\n");
    enable_interrupt();
}

void curr_irq_handler()
{
    disable_interrupt();
    unsigned int uart = (*IRQ_PENDING_1 & AUX_IRQ);
    if (uart)
    {
        uart_handler();
    }
    enable_interrupt();
}

void curr_sync_handler(unsigned long esr_el1, unsigned long elr_el1)
{
    disable_interrupt();
    return;
    enable_interrupt();
}
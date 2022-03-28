#include "utils_s.h"
#include "mini_uart.h"
#include "timer.h"
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

void lower_irq_handler()
{
    unsigned long current_time = get_current_time();
    uart_send_string("After booting: ");
    uart_dec(current_time);
    uart_send_string(" seconds \n\n");
    set_expired_time(2);
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
    unsigned int core_timer = (*CORE0_INTERRUPT_SOURCE & 0x2);
    if (uart)
    {
        uart_handler();
    }
    else if (core_timer)
    {
        unsigned long current_time = get_current_time();
        uart_send_string("\nmessage :");
        timeout_queue_head->callback(timeout_queue_head->msg);
        uart_send_string("\ncurrent time :");
        uart_dec(current_time);
        uart_send_string("s\n");
        uart_send_string("command executed time :");
        uart_dec(timeout_queue_head->register_time);
        uart_send_string("s\n");
        uart_send_string("command duration time :");
        uart_dec(timeout_queue_head->duration);
        uart_send_string("s\n\n");

        timeout_event *next = timeout_queue_head->next;
        if (next)
        {
            next->prev = 0;
            timeout_queue_head = next;
            set_expired_time(next->register_time + next->duration - get_current_time());
        }
        else // no other event
        {
            timeout_queue_head = timeout_queue_tail = 0;
            core_timer_disable();
        }
    }
    enable_interrupt();
}

void curr_sync_handler(unsigned long esr_el1, unsigned long elr_el1)
{
    disable_interrupt();
    return;
    enable_interrupt();
}
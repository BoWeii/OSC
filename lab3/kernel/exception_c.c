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
    uart_printf("spsr_el1: %x\n",spsr);
    uart_printf("elr_el1: %x\n",elr);
    uart_printf("esr_el1: %x\n\n",esr);
    enable_interrupt();
}

void lower_irq_handler()
{
    unsigned long current_time = get_current_time();
    uart_printf("After booting: %d seconds\n\n",current_time);
    set_expired_time(2);
}

void lower_sync_handler()
{
    disable_interrupt();
    unsigned long spsr = read_sysreg(spsr_el1);
    unsigned long elr = read_sysreg(elr_el1);
    unsigned long esr = read_sysreg(esr_el1);
    uart_printf("spsr_el1: %x\n",spsr);
    uart_printf("elr_el1: %x\n",elr);
    uart_printf("esr_el1: %x\n\n",esr);
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
        uart_printf("\ncurrent time : %ds\n",current_time);
        uart_printf("command executed time : %ds\n",timeout_queue_head->register_time);
        uart_printf("command duration time : %ds\n\n",timeout_queue_head->duration);

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
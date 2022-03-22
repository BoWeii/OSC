#include "utils_s.h"
#include "mini_uart.h"

void svc_handler() {
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
}
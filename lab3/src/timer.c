#include "timer.h"
#include "utils_s.h"
#include "mini_uart.h"

void core_timer_enable()
{
    /*
        cntpct_el0 >= cntp_cval_el0 -> interrupt
        cntp_tval_el0 = cntpct_el0 - cntp_cval_el0
    */
    write_sysreg(cntp_ctl_el0, 1); // enable
    unsigned long frq = read_sysreg(cntfrq_el0);
    write_sysreg(cntp_tval_el0, frq * 2); // set expired time
    *CORE0_TIMER_IRQ_CTRL = 2;            // unmask timer interrupt
}

void core_timer_handler()
{
    // cntpct_el0: The timer’s current count.
    unsigned long frq = read_sysreg(cntfrq_el0);
    write_sysreg(cntp_tval_el0, frq * 2);
    unsigned long current_count = read_sysreg(cntpct_el0);

    uart_send_string("After booting: ");
    uart_hex((unsigned long)(current_count / frq));
    uart_send_string(" seconds \n\n");
}
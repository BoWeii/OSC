#include "timer.h"
#include "utils_s.h"
#include "mm.h"
#include "mini_uart.h"
#include "utils_c.h"
#include "thread.h"
#include "exception_c.h"
#include "current.h"

timeout_event *timeout_queue_head, *timeout_queue_tail;

void core_timer_enable()
{
    /*
        cntpct_el0 >= cntp_cval_el0 -> interrupt
        cntp_tval_el0 = cntpct_el0 - cntp_cval_el0
    */
    write_sysreg(cntp_ctl_el0, 1); // enable
    unsigned long frq = read_sysreg(cntfrq_el0);
    write_sysreg(cntp_tval_el0, frq * 1); // set expired time
    *CORE0_TIMER_IRQ_CTRL = 2;            // unmask timer interrupt
}

void core_timer_disable()
{
    write_sysreg(cntp_ctl_el0, 0); // disable
    *CORE0_TIMER_IRQ_CTRL = 0;     // unmask timer interrupt
}

void set_expired_time(unsigned long duration)
{
    unsigned long frq = read_sysreg(cntfrq_el0) / 1000;
    write_sysreg(cntp_tval_el0, frq * duration); // ms
}

unsigned long get_current_time()
{
    // cntpct_el0: The timer’s current count.
    unsigned long frq = read_sysreg(cntfrq_el0) / 1000;
    unsigned long current_count = read_sysreg(cntpct_el0);
    return (unsigned long)(current_count / frq);
}

void set_timeout(char *message, unsigned long time)
{
    add_timer((timer_callback)print_message, (size_t)message, time);
}

void print_message(char *msg)
{
    unsigned long current_time = get_current_time();
    uart_printf("\ncurrent time : %d.%ds\n", GET_S(current_time), GET_MS(current_time));
    uart_printf("message: %s\n\n", msg);
}

void timeout_event_init()
{
    timeout_queue_head = 0;
    timeout_queue_tail = 0;
    add_timer((timer_callback)thread_schedule, (size_t)NULL, MS(SCHE_CYCLE));
}

void add_timer(timer_callback cb, size_t arg, unsigned long duration)
{
    timeout_event *new_event = (timeout_event *)kmalloc(sizeof(timeout_event));
    new_event->register_time = get_current_time();
    new_event->duration = duration;
    new_event->callback = cb;
    new_event->arg = arg;
    new_event->next = 0;
    new_event->prev = 0;
    size_t flag = disable_irq();
    if (timeout_queue_head == 0)
    {
        timeout_queue_head = new_event;
        timeout_queue_tail = new_event;
        core_timer_enable();
        set_expired_time(duration);
    }
    else
    {
        unsigned long timeout = new_event->register_time + new_event->duration;
        timeout_event *cur = timeout_queue_head;
        while (cur)
        {
            if ((cur->register_time + cur->duration) > timeout)
                break;
            cur = cur->next;
        }

        if (cur == 0)
        { // cur at end
            new_event->prev = timeout_queue_tail;
            timeout_queue_tail->next = new_event;
            timeout_queue_tail = new_event;
        }
        else if (cur->prev == 0)
        { // cur at head
            new_event->next = cur;
            (timeout_queue_head)->prev = new_event;
            timeout_queue_head = new_event;
            set_expired_time(duration);
        }
        else
        { // cur at middle
            new_event->next = cur;
            new_event->prev = cur->prev;
            (cur->prev)->next = new_event;
            cur->prev = new_event;
        }
    }
    irq_restore(flag);
}

static inline void set_resched(unsigned long current_time)
{
    if (current_time >= current->timeout)
    {
        current->need_resched = 1;
    }
}

void timer_handler()
{
    unsigned long current_time = get_current_time();
    timeout_event *next_event = timeout_queue_head->next;
    timeout_event *cur_event = timeout_queue_head;
    if (next_event)
    {
        next_event->prev = 0;
        timeout_queue_head = next_event;
        set_expired_time(next_event->register_time + next_event->duration - current_time);
    }
    else // no other eventwee
    {
        timeout_queue_head = timeout_queue_tail = 0;
        // core_timer_disable();
    }
    set_resched(current_time);
    if (cur_event->callback != NULL)
    {
        if ((cur_event->callback) == (timer_callback)thread_schedule)
        {
            add_timer((timer_callback)thread_schedule, (size_t)NULL, MS(SCHE_CYCLE));
            enable_interrupt();
            cur_event->callback(cur_event->arg);
            disable_interrupt();
        }
        else
        {
            cur_event->callback(cur_event->arg);
        }
    }
    kfree(cur_event);
}
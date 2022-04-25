#ifndef __TIMER_H
#define __TIMER_H
#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int *)(0x40000040))
#define CORE0_INTERRUPT_SOURCE ((volatile unsigned int *)(0x40000060))
#define DEFAULT_TIMEOUT 30LL
#define SCHE_CYCLE 30LL
#define SYSTEM_TIMER_MSG "system_timeout"
#define S(n) (n*1000LL)
#define MS(n) (n*1LL)
#define GET_S(n) (n/1000LL)
#define GET_MS(n) (n%1000LL)
#include "stddef.h"

typedef void (*timer_callback)(size_t);

void core_timer_enable();
void core_timer_disable();
void set_expired_time(unsigned long duration);
unsigned long get_current_time();
void core_timer_handler();
void set_timeout(char *message, unsigned long time);
void print_message(char *msg);
void add_timer(timer_callback cb, size_t arg, unsigned long duraction);
void timeout_event_init();
void timer_handler();

typedef struct timeout_event
{
  unsigned long register_time;
  unsigned long duration;
  timer_callback callback;
  size_t arg;
  struct timeout_event *prev, *next;
} timeout_event;

extern timeout_event *timeout_queue_head, *timeout_queue_tail;

#endif

#ifndef _EXCEPTION_C_H
#define _EXCEPTION_C_H

void enable_interrupt();
void disable_interrupt();
void default_handler();
void lower_sync_handler();
void curr_irq_handler();
void curr_sync_handler();
#endif

#ifndef _CURRENT_H
#define _CURRENT_H

#include "thread.h"

static inline task *get_thread_ds()
{
    return (task *)read_sysreg(tpidr_el1);
}
static inline void set_thread_ds(task *cur)
{
    write_sysreg(tpidr_el1, cur);
}
#define current get_thread_ds()

#endif 

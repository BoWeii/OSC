#ifndef __SYSCALL_H
#define __SYSCALL_H

#include "trap_frame.h"

#define _STR(x) #x
#define STR(s) _STR(s)

typedef void (*syscall)(TrapFrame *);

enum {
    SYS_GETPID,
    SYS_UART_READ,
    SYS_UART_WRITE,
    SYS_EXEC,
    SYS_FORK,
    SYS_EXIT,
    SYS_MBOX,
    SYS_KILL,
    NUM_syscalls
};

extern syscall syscall_table[];
void syscall_handler(TrapFrame *regs);

#endif
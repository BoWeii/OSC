#include "syscall.h"
#include "stddef.h"
#include "trap_frame.h"
#include "current.h"
#include "mini_uart.h"
#include "exec.h"

void sys_getpid(TrapFrame *_regs)
{
    uart_send_string("in sys_getpid\n");
    _regs->regs[0] = current->pid;
}
void sys_uartread(TrapFrame *_regs)
{
}
void sys_uartwrite(TrapFrame *_regs)
{
}
void sys_exec(TrapFrame *_regs)
{
}
void sys_fork(TrapFrame *_regs)
{
}
void sys_exit(TrapFrame *_regs)
{
    uart_send_string("in sys_exit\n");
}
void sys_mbox_call(TrapFrame *_regs)
{
}
void sys_kill_pid(TrapFrame *_regs)
{
}

syscall syscall_table[NUM_syscalls] = {
    [SYS_GETPID] = &sys_getpid,
    [SYS_UART_READ] = &sys_uartread,
    [SYS_UART_WRITE] = &sys_uartwrite,
    [SYS_EXEC] = &sys_exec,
    [SYS_FORK] = &sys_fork,
    [SYS_EXIT] = &sys_exit,
    [SYS_MBOX] = &sys_mbox_call,
    [SYS_KILL] = &sys_kill_pid,
};

void syscall_handler(TrapFrame *_regs)
{
    unsigned int sys_index = _regs->regs[8];
    if (sys_index >= NUM_syscalls)
    {
        uart_send_string("!!! Invalid system call !!!\n");
        return;
    }
    (syscall_table[sys_index])(_regs);
}

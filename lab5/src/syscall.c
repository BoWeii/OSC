#include "syscall.h"
#include "stddef.h"
#include "trap_frame.h"
#include "current.h"
#include "peripheral/mailbox.h"
#include "mini_uart.h"
#include "exec.h"

void sys_getpid(TrapFrame *_regs)
{
    _regs->regs[0] = current->pid;
}
void sys_uartrecv(TrapFrame *_regs)
{
    char *buf = (char *)_regs->regs[0];
    int count = _regs->regs[1];
    for (int i = 0; i < count; i++)
    {
        buf[i] = uart_recv();
    }
    _regs->regs[0] = count;
}
void sys_uartwrite(TrapFrame *_regs)
{
    char *buf = (char *)_regs->regs[0];
    int count = _regs->regs[1];
    for (int i = 0; i < count; i++)
    {
        uart_send(buf[i]);
    }
    _regs->regs[0] = count;
}
void sys_exec(TrapFrame *_regs)
{
    const char *path = (char *)_regs->regs[0];
    const char **args = (const char **)_regs->regs[1];
    _regs->regs[0]=do_exec(path,args);
}
void sys_fork(TrapFrame *_regs)
{
}
void sys_exit(TrapFrame *_regs)
{
    kill_task(current, _regs->regs[0]);
}
void sys_mbox_call(TrapFrame *_regs)
{
    unsigned int channel = _regs->regs[0];
    unsigned int *mailbox = (unsigned int*)_regs->regs[1];
    mailbox_call(channel, mailbox);
}
void sys_kill_pid(TrapFrame *_regs)
{
    pid_t target = _regs->regs[0];
    if (current->pid == target)
    {
        kill_task(current, target);
        return;
    }
    struct task *victim = get_task(target);
    if (victim)
    {
        kill_task(victim, 0);
    }
}

syscall syscall_table[NUM_syscalls] = {
    [SYS_GETPID] = &sys_getpid,
    [SYS_UART_RECV] = &sys_uartrecv,
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

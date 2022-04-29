#include "fork.h"
#include "stddef.h"
#include "sche.h"
#include "thread.h"
#include "exception_c.h"
#include "exception_s.h"
#include "utils_c.h"
#include "current.h"
#include "mm.h"

static struct task *fork_context(TrapFrame *_regs)
{
    struct task *_task = kmalloc(sizeof(struct task));

    unsigned long flags = disable_irq();
    *_task = *current; // copy the current to _task entirely
    _task->pid = task_count++;
    irq_restore(flags);

    _task->need_resched = 0;

    _task->user_stack = kmalloc(STACK_SIZE);
    memcpy(_task->user_stack, current->user_stack, STACK_SIZE);

    _task->kernel_stack = kmalloc(STACK_SIZE);
    TrapFrame *trapframe = (TrapFrame *)((unsigned long)_task->kernel_stack + STACK_SIZE - sizeof(TrapFrame));
    memcpy(trapframe, _regs, sizeof(TrapFrame));

    _task->user_prog = kmalloc(current->user_prog_size);
    memcpy(_task->user_prog, current->user_prog, current->user_prog_size);

    trapframe->sp = (unsigned long)_task->user_stack + (_regs->sp - (unsigned long)current->user_stack);
    trapframe->pc = (unsigned long)_task->user_prog + (_regs->pc - (unsigned long)current->user_prog);
    trapframe->regs[0] = 0; // child process : return 0

    _task->cpu_context.sp = (unsigned long)trapframe;
    _task->cpu_context.lr = (unsigned long)restore_regs_eret;
    return _task;
}

size_t do_fork(TrapFrame *_regs)
{
    struct task *_task = fork_context(_regs);
    add_task(_task);
    return _task->pid;
}

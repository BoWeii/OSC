#include "sche.h"
#include "current.h"
#include "timer.h"
#include "exception_c.h"
#include "mini_uart.h"
#include "mm.h"

pid_t task_count = 0;

list running_queue = LIST_HEAD_INIT(running_queue);
list waiting_queue = LIST_HEAD_INIT(waiting_queue);
list stopped_queue = LIST_HEAD_INIT(stopped_queue);

void add_task(struct task *t)
{
    size_t flags = disable_irq();
    insert_tail(&running_queue, &t->list);
    irq_restore(flags);
}

void kill_task(struct task *_task, int status)
{
    size_t flags = disable_irq();

    _task->state = TASK_STOPPED;
    _task->need_resched = 1;
    unlink(&_task->list);

    _task->exitcode = status;
    insert_head(&stopped_queue, &_task->list);

    irq_restore(flags);
}

void restart_task(struct task *_task)
{
    size_t flags = disable_irq();

    if (_task->state != TASK_WAITING)
    {
        uart_send_string("---- task state inconsistent ----\n");
        return;
    }
    _task->state = TASK_RUNNING;
    unlink(&_task->list);
    insert_tail(&running_queue, &_task->list);
    irq_restore(flags);
}

void pause_task(struct task *_task)
{
    size_t flags = disable_irq();

    if (_task->state != TASK_RUNNING)
    {
        uart_send_string("---- task state inconsistent ----\n");
        return;
    }
    _task->state = TASK_WAITING;
    _task->need_resched = 1;
    unlink(&_task->list);
    insert_head(&waiting_queue, &_task->list);

    irq_restore(flags);
}

void sleep_task(size_t ms)
{
    size_t flags = disable_irq();
    add_timer((timer_callback)restart_task, (size_t)current, ms);
    pause_task(current);
    irq_restore(flags);
    thread_schedule(0);
}

void free_task(struct task *victim)
{
    if (victim->kstack)
        kfree(victim->kstack);
    kfree(victim);
}

struct task *create_task()
{
    struct task *new_task = kmalloc(sizeof(struct task));
    new_task->kstack = NULL;
    new_task->state = TASK_INIT;
    new_task->pid = task_count++;
    new_task->need_resched = 0;
    new_task->timeout = get_current_time() + DEFAULT_TIMEOUT;
    new_task->cpu_context.lr = 0;
    new_task->cpu_context.sp = 0;
    return new_task;
}

struct task *pick_next_task()
{
    if (list_empty(&running_queue))
    {
        while (1)
        {
            uart_send_string("scheduler: run queue is empty\n");
        };
    }
    struct task *next_task = list_first_entry(&running_queue, struct task, list);
    unlink(&next_task->list);
    insert_tail(&running_queue, &next_task->list);

    return next_task;
}

void switch_task(struct task *next)
{
    if (current == next)
    {
        return;
    }
    switch_to(&current->cpu_context, &next->cpu_context);
}
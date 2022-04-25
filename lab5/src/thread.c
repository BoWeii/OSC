#include "thread.h"
#include "mini_uart.h"
#include "timer.h"
#include "mm.h"
#include "exception_c.h"
#include "current.h"

static int task_count = 0;

list running_queue = LIST_HEAD_INIT(running_queue);
list waiting_queue = LIST_HEAD_INIT(waiting_queue);
list stopped_queue = LIST_HEAD_INIT(stopped_queue);

////////////////////////////////////////////////////////////////////////////////////////////////
//                                       schedule                                             //
////////////////////////////////////////////////////////////////////////////////////////////////
struct pt_regs
{
    unsigned long regs[31];
    unsigned long sp;
    unsigned long pc;
    unsigned long pstate;
};

int get_the_cur_count()
{
    int count = 0;
    list *head = &running_queue;
    while (head->next != &running_queue)
    {
        count++;
        head = head->next;
    }
    return count;
}

void add_task(task *t)
{
    size_t flags = disable_irq();
    insert_tail(&running_queue, &t->list);
    irq_restore(flags);
}

void kill_task(task *_task, int status)
{
    size_t flags = disable_irq();

    _task->state = TASK_STOPPED;
    _task->need_resched = 1;
    unlink(&_task->list);

    _task->exitcode = status;
    insert_head(&stopped_queue, &_task->list);

    irq_restore(flags);
}

void restart_task(task *_task)
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

void pause_task(task *_task)
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
    thread_schedule();
}

task *pick_next_task()
{
    if (list_empty(&running_queue))
    {
        while (1)
        {
            uart_send_string("scheduler: run queue is empty\n");
        };
    }
    task *next_task = list_first_entry(&running_queue, task, list);
    unlink(&next_task->list);
    insert_tail(&running_queue, &next_task->list);

    return next_task;
}
////////////////////////////////////////////////////////////////////////////////////////////////
//                                       thread                                               //
////////////////////////////////////////////////////////////////////////////////////////////////


static void switch_task(task *next)
{
    switch_to(&current->cpu_context, &next->cpu_context);
}

void thread_schedule()
{
    if (!current->need_resched)
    {
        return;
    }

    size_t flags = disable_irq();

    task *next = pick_next_task();
    next->timeout = get_current_time() + DEFAULT_TIMEOUT;
    next->need_resched = 0;

    irq_restore(flags);
    switch_task(next);
}

void thread_init()
{
    task *cur = kmalloc(sizeof(task));
    cur->state = TASK_RUNNING;
    cur->pid = task_count++;
    cur->need_resched = 0;
    cur->timeout = get_current_time() + DEFAULT_TIMEOUT;
    set_thread_ds(cur); // init the thread structure
}

static void foo()
{
    for (int i = 0; i < 5; ++i)
    {
        uart_printf("Thread id: %d i=%d\n", current->pid, i);
        // size_t flags = read_sysreg(DAIF);
        // uart_printf("in thread daif=%x\n", flags & 0b1111000000);
        sleep_task(500);
    }
    kill_task(current, 0);
    thread_schedule();
}

task *thread_create(void *func)
{
    task *new_task = kmalloc(sizeof(task));
    new_task->kstack = kmalloc(0x2000);
    new_task->state = TASK_RUNNING;
    new_task->pid = task_count++;
    new_task->need_resched = 0;
    new_task->timeout = get_current_time() + DEFAULT_TIMEOUT;
    new_task->cpu_context.lr = (unsigned long)func;
    new_task->cpu_context.sp = (unsigned long)new_task->kstack + 0x2000;

    add_task(new_task);
    return new_task;
}

void test_thread()
{
    for (int i = 0; i < 3; ++i)
    {
        thread_create(&foo);
    }
    uart_printf("end of test_thread\n");
}
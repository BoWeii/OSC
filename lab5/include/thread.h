#ifndef _THREAD_H
#define _THREAD_H
#include "list.h"
#include "utils_s.h"


typedef unsigned long pid_t;
typedef enum
{
    TASK_RUNNING,
    TASK_WAITING,
    TASK_STOPPED
} state_t;

typedef struct cpu_context
{
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long fp;
    unsigned long sp;
    unsigned long lr;
} cpu_context;

typedef struct task
{
    cpu_context cpu_context;
    pid_t pid;
    state_t state;
    list list;
    unsigned need_resched;
    int exitcode;
    // char *stack;
    char *kstack;
    unsigned long timeout;
} task;

void test_thread();
task *thread_create(void *func);
void idle();
void thread_schedule();
void add_task(task *t);
void thread_init();
int get_the_cur_count();



#endif

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

struct cpu_context
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
};

struct task
{
    struct cpu_context cpu_context;
    pid_t pid;
    state_t state;
    list list;
    unsigned need_resched;
    int exitcode;
    char *kstack;
    unsigned long timeout;
};

void test_thread();
struct task *thread_create(void *func);
void thread_schedule(size_t _);
void add_task(struct task *t);
void thread_init();
int get_the_cur_count();



#endif

#include "mini_uart.h"
#include "dtb.h"
#include "exception_c.h"
#include "utils_s.h"
#include "shell.h"
#include "mm.h"
#include "timer.h"
#include "thread.h"
extern void *_dtb_ptr;

void print_shit(){
    int t=0;
    while(1){
        uart_printf("in shit %d,cur task count=%d\n",t++,get_the_cur_count());
    }
}

void kernel_main(void)
{
    // uart_init();
    uart_send_string("Hello, world!\n");
    mm_init();
    thread_init();
    thread_create(&shell);
    // thread_create(&print_shit);
    timeout_event_init();
    enable_interrupt();

}
#include "peripheral/mailbox.h"
#include "utils_string.h"
#include "mini_uart.h"

volatile unsigned int  __attribute__((aligned(16))) mailbox[36];

int mailbox_call() {
    // uart_send_string("before while \r\n");

    unsigned int readChannel = (((unsigned int)((unsigned long)&mailbox)&~0xF) | (0x8 &0xF));
    while ( *MAILBOX_STATUS & MAILBOX_FULL ) {}
    *MAILBOX_WRITE = readChannel;
    while (1) {
        while (*MAILBOX_STATUS & MAILBOX_EMPTY) {}
        if(readChannel==*MAILBOX_READ){
            return mailbox[1]==MAILBOX_RESPONSE;
        }
    }
    return 0;
}


void get_board_revision() {
    uart_send_string("In get_board_revision\r\n");
    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_BOARD_REVISION; // tag identifier
    mailbox[3] = 4; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    // tags end
    mailbox[6] = END_TAG;
    unsigned int a = mailbox_call(); // message passing procedure call, you should implement it following the 6 steps provided above.
    char str[256];
    // printf("0x%x\n", mailbox[5]); // it should be 0xa020d3 for rpi3 b+
    utils_uint2str_hex(mailbox[5], str);
    uart_send_string(str);
    uart_send_string("\r\n");
}


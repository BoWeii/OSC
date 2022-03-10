.section ".text.relo"
.globl _start
# need to relocate the bootloader from 0x80000 to 0x60000
_start:
    adr x10, .          //x10=0x80000
    ldr x11, =_blsize 
    add x11, x11, x10 
    ldr x12, =_stext    // x12=0x60000

moving_relo:
    cmp x10, x11        //without bootloader
    b.eq end_relo
    ldr x13, [x10]
    str x13, [x12]      //move 0x80000 data to 0x60000
    add x12, x12, #8
    add x10, x10, #8
    b moving_relo
end_relo:
    ldr x14, =_bl_entry    //jump to boot part 
    br x14


.section ".text.boot"
.globl _start_bl
    mrs    x0, mpidr_el1        
    and    x0, x0,#0xFF // Check processor id
    cbz    x0, master   // Hang for all non-primary CPU

hang:
    b hang

master:
    adr    x0, _sbss
    adr    x1, _ebss
    sub    x1, x1, x0
    bl     memzero

    mov    sp, #0x400000    // 4MB
    bl    bootloader_main
    
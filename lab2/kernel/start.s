.section ".text.kernel"

.globl _start
_start:
    mrs    x0, mpidr_el1        
    and    x0, x0,#0xFF        // Check processor id
    cbz    x0, master        // Hang for all non-primary CPU

hang:
    b hang

master:
    adr    x0, _sbss
    adr    x1, _ebss
    sub    x1, x1, x0
    bl     memzero

    mov    sp, #0x400000  // 4MB
    bl    kernel_main
    
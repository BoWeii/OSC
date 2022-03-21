.globl from_el2_to_el1
from_el2_to_el1:
    mov x2, (1 << 31)   // EL1 uses aarch64
    msr hcr_el2, x2
    mov x2, 0x3c5       // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x2
    msr elr_el2, lr
    eret                // return to EL1
    
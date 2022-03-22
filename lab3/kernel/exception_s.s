// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm


_el1_lower_el_aarch64_sync:
  save_all
  bl svc_handler
  load_all
  eret

_el1_lower_el_aarch64_irq:
  save_all
  bl core_timer_handler
  load_all
  eret

.global el1_vector_base
.section ".vector"


.balign 0x800
el1_vector_base:
el1_curr_el_sp0_sync:
    b el1_curr_el_sp0_sync

.balign 0x80
el1_curr_el_sp0_irq:
    b el1_curr_el_sp0_irq

.balign 0x80
el1_curr_el_sp0_fiq:
    b el1_curr_el_sp0_fiq

.balign 0x80
el1_curr_el_sp0_serror:
    b el1_curr_el_sp0_serror

.balign 0x80
el1_curr_el_spx_sync:
    b el1_curr_el_spx_sync

.balign 0x80
el1_curr_el_spx_irq:
    b el1_curr_el_spx_irq

.balign 0x80
el1_curr_el_spx_fiq:
    b el1_curr_el_spx_fiq

.balign 0x80
el1_curr_el_spx_serror:
    b el1_curr_el_spx_serror

.balign 0x80
el1_lower_el_aarch64_sync:
    b _el1_lower_el_aarch64_sync

.balign 0x80
el1_lower_el_aarch64_irq:
    b _el1_lower_el_aarch64_irq

.balign 0x80
el1_lower_el_aarch64_fiq:
    b el1_lower_el_aarch64_fiq

.balign 0x80
el1_lower_el_aarch64_serror:
    b el1_lower_el_aarch64_serror

.balign 0x80
el1_lower_el_aarch32_sync:
    b el1_lower_el_aarch32_sync

.balign 0x80
el1_lower_el_aarch32_irq:
    b el1_lower_el_aarch32_irq

.balign 0x80
el1_lower_el_aarch32_fiq:
    b el1_lower_el_aarch32_fiq

.balign 0x80
el1_lower_el_aarch32_serror:
    b el1_lower_el_aarch32_serror

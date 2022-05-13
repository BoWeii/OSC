#ifndef __MMU_H_
#define __MMU_H_

typedef unsigned long pd_t;



#define PHYS_OFFSET 0xffff000000000000

static inline void *phys_to_virt(unsigned long p) {
    return (void *)(p + PHYS_OFFSET);
}

static inline unsigned long virt_to_phys(void *p) {
    return (unsigned long)p - PHYS_OFFSET;
}

#define IDENTITY_TT_L0 ((pd_t *)0x1000L)
#define IDENTITY_TT_L1 ((pd_t *)0x2000L)
#define IDENTITY_TT_L0_VA ((pd_t *)phys_to_virt(0x1000L))
#define IDENTITY_TT_L1_VA ((pd_t *)phys_to_virt(0x2000L))

void setup_identity_mapping();
void setup_kernel_space_mapping();
#endif

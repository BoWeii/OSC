#include "mmu.h"
#include "utils_s.h"
#include "utils_c.h"
#include "mini_uart.h"
#include "mm.h"

#define TCR_CONFIG_REGION_48bit (((64 - 48) << 0) | ((64 - 48) << 16))
#define TCR_CONFIG_4KB ((0b00 << 14) | (0b10 << 30))
#define TCR_CONFIG_DEFAULT (TCR_CONFIG_REGION_48bit | TCR_CONFIG_4KB)

#define MAIR_DEVICE_nGnRnE 0b00000000
#define MAIR_NORMAL_NOCACHE 0b01000100
#define MAIR_IDX_DEVICE_nGnRnE 0
#define MAIR_IDX_NORMAL_NOCACHE 1

#define PD_TABLE 0b11
#define PD_BLOCK 0b01
#define PD_ACCESS (1 << 10)
#define BOOT_PGD_ATTR PD_TABLE
#define BOOT_PUD_ATTR (PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK)
#define MMU_ENABLE 1
#define PD_NS (1 << 5)
#define PAGE_SIZE 0x1000

void setup_identity_mapping()
{
    write_sysreg(tcr_el1, TCR_CONFIG_DEFAULT);
    size_t mair_attr_01 =
        (MAIR_DEVICE_nGnRnE << (MAIR_IDX_DEVICE_nGnRnE * 8)) |
        (MAIR_NORMAL_NOCACHE << (MAIR_IDX_NORMAL_NOCACHE * 8));
    write_sysreg(mair_el1, mair_attr_01);

    memset(IDENTITY_TT_L0, 0, 0x1000);
    memset(IDENTITY_TT_L1, 0, 0x1000);

    IDENTITY_TT_L0[0] = (pd_t)IDENTITY_TT_L1 | BOOT_PGD_ATTR;
    IDENTITY_TT_L1[0] = 0x00000000 | BOOT_PUD_ATTR;
    IDENTITY_TT_L1[1] = 0x40000000 | BOOT_PUD_ATTR;

    write_sysreg(ttbr0_el1, IDENTITY_TT_L0);
    write_sysreg(ttbr1_el1, IDENTITY_TT_L0);    // also load PGD to the upper translation based register.
    unsigned long sctlr = read_sysreg(sctlr_el1);
    write_sysreg(sctlr_el1, sctlr | MMU_ENABLE);
}

void setup_kernel_space_mapping() {
    /*  three-level 2MB block mapping    */

    /*  0x00000000 ~ 0x3F000000 for normal mem  */
    pd_t *p0 = kcalloc(PAGE_SIZE);
    for (int i = 0; i < 504; i++) {
        p0[i] = (i << 21) | PD_ACCESS | (MAIR_IDX_NORMAL_NOCACHE << 2)  | PD_BLOCK;
    }
    /*  0x3F000000 ~ 0x40000000 for device mem  */
    for (int i = 504; i < 512; i++) {
        p0[i] = (i << 21) | PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK;
    }

    /*  0x40000000 ~ 0x80000000 for device mem  */
    pd_t *p1 = kcalloc(PAGE_SIZE);
    for (int i = 0; i < 512; i++) {
        p1[i] = 0x40000000 | (i << 21) | PD_ACCESS | (MAIR_IDX_DEVICE_nGnRnE << 2) | PD_BLOCK;
    }

    asm volatile("dsb ish\n\t");         /* ensure write has completed */

    IDENTITY_TT_L1_VA[0] = (pd_t)virt_to_phys(p0) | PD_TABLE;
    IDENTITY_TT_L1_VA[1] = (pd_t)virt_to_phys(p1) | PD_TABLE;

}
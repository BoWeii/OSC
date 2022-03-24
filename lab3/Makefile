CFLAG = -Iinclude -fno-stack-protector
K8=kernel8
BL=bootloader
LINKER=linker.ld
K=kernel
START=start
CPIO:=archive/initramfs.cpio
DTB:=archive/bcm2710-rpi-3-b-plus.dtb
ARC:=app

SRC_C:=$(shell find src/ -name '*.c')
SRC_S:=$(shell find src/ -name '*.s')

KER_C:=$(shell find kernel/ -name '*.c')
KER_S:=$(shell find kernel/ -name '*.s')

BL_C:=$(shell find bootloader/ -name '*.c')
BL_S:=$(shell find bootloader/ -name '*.s')

ARC_S:=$(shell find archive/ -name '*.s')

ARC_OBJS:= $(ARC_S:%.s=%.o) 

OBJS:= $(SRC_C:%.c=%.o) \
		$(SRC_S:%.s=%.o)\


KERNEL_OBJS:=$(KER_C:%.c=%.o) \
		$(KER_S:%.s=%.o)\
		rd.o

BL_OBJS:=$(BL_C:%.c=%.o) \
		$(BL_S:%.s=%.o) \
		src/mini_uart.o src/utils_c.o src/utils_s.o src/memzero.o




all: $(ARC).img $(K8).img $(BL).img 



rd.o:$(CPIO)
	aarch64-linux-gnu-ld -r -b binary -o rd.o $(CPIO)

qe:$(K8).img
	qemu-system-aarch64 -M raspi3 -kernel $< -display none -serial null -serial stdio -s -initrd $(CPIO) -dtb $(DTB)


%.o: %.s
	aarch64-linux-gnu-gcc -g -o $@ -c $<

%.o: %.c
	aarch64-linux-gnu-gcc -g -o $@ $(CFLAG) -c $<


$(K8).img: $(OBJS) $(KERNEL_OBJS)
	aarch64-linux-gnu-ld -T kernel/linker.ld -o $(K8).elf $^
	aarch64-linux-gnu-objcopy -O binary $(K8).elf $@

$(BL).img: $(BL_OBJS) 
	aarch64-linux-gnu-ld -T bootloader/linker.ld -o $(BL).elf $^
	aarch64-linux-gnu-objcopy -O binary $(BL).elf $@

$(ARC).img:$(ARC_OBJS)
	aarch64-linux-gnu-ld -T archive/linker.ld -o archive/tmp.elf $<
	aarch64-linux-gnu-objcopy -O binary archive/tmp.elf $@
	cp $@ archive/rootfs/$@ 

$(CPIO):
	cd archive/rootfs&&find . | cpio -o -H newc > ../initramfs.cpio

on:
	sudo screen /dev/ttyUSB0 115200
clean:
	$(RM) $(K8:=.elf) $(BL:=.elf) $(OBJS) $(KERNEL_OBJS) $(BL_OBJS) $(K8:=.img) $(BL:=.img) \
	archive/initramfs.cpio archive/rootfs/$(ARC).img archive/$(ARC).o archive/tmp.elf
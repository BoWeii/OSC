.PHONY:all clean
LINKER:=linker.ld
CFLAGS:=-nostdinc -nostdlib -nostartfiles -no-stack-protector

all:
	make clean
	make app.img
	cd rootfs&&find . | cpio -o -H newc > ../initramfs.cpio

app.img:app.o
	aarch64-linux-gnu-ld -T $(LINKER) -o tmp.elf $<
	aarch64-linux-gnu-objcopy -O binary tmp.elf $@
	cp $@ ./rootfs/$@
	rm $@
	rm *.o

app.o:app.s
	aarch64-linux-gnu-gcc -g -o $@ $(CFLAG) -c $<

clean:
	-rm *.cpio &rm rootfs/app.img

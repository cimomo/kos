CC=gcc
CFLAGS= -Wall -c -I
CFLAGS2 = -c -O2
AS=nasm
#ASFLAGS=-f elf
ASFLAGS= -f aout 
SIZE=$(shell ./src/util/sc kernel.bin)
OBJS=kernel.o kio.o isr.o kbd.o util.o mm.o paging.o vdesktop.o console.o kstring.o login.o sched.o hello.o #interrupts.o
INCLUDES=include/*.h
SOURCES=*.c
INCLUDE=src/include
KERNEL=src/kernel
BOOT=src/boot
UTIL=src/util
ASM=src/asm
LIB=src/lib
MM=src/mm

all:    kos.fd0

#include .deps

#.deps:  $(SOURCES) $(INCLUDES)
#	@makedepend -f- $(CFLAGS) -Y $(SOURCES) 1>$@ 2>/dev/null

#kernel.o: kernel.c ./include/stdio.h
#	$(CC) -c kernel.c

#kernel:	$(OBJS)
#	ld $^ -Ttext 0x10000 -e main -N -o $@

#kernel.bin: kernel
#	objcopy -R .note -R .comment -S -O binary $< $@

boot.o:	$(BOOT)/boot.s $(UTIL)/sc kernel.bin
	$(AS) -D KERNEL_SIZE=$(SIZE) -o $@ $<

kio.o: $(KERNEL)/kio.c $(INCLUDE)/kio.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/kio.c -o $@

isr.o: $(KERNEL)/isr.c $(INCLUDE)/isr.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/isr.c -o $@

kbd.o: $(KERNEL)/kbd.c $(INCLUDE)/kbd.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/kbd.c -o $@

stdio.o: $(LIB)/stdio.c $(INCLUDE)/stdio.h
	$(CC) $(CFLAGS)$(INCLUDE) $(LIB)/stdio.c -o $@

mm.o:	$(MM)/mm.c $(INCLUDE)/mm.h
	$(CC) $(CFLAGS)$(INCLUDE) $(MM)/mm.c -o $@

paging.o: $(MM)/paging.c $(INCLUDE)/mm.h
	$(CC) $(CFLAGS)$(INCLUDE) $(MM)/paging.c -o $@

vdesktop.o: $(KERNEL)/vdesktop.c $(INCLUDE)/vdesktop.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/vdesktop.c -o $@

console.o: $(KERNEL)/console.c $(INCLUDE)/console.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/console.c -o $@

kstring.o: $(KERNEL)/kstring.c $(INCLUDE)/kstring.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/kstring.c -o $@

login.o: $(KERNEL)/login.c $(INCLUDE)/login.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/login.c -o $@

sched.o: $(KERNEL)/sched.c $(INCLUDE)/sched.h
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/sched.c -o $@

util.o: $(ASM)/util.s
	$(AS) $(ASFLAGS) $(ASM)/util.s -o $@

interrupts.o: $(KERNEL)/interrupts.s
	$(AS) $(ASFLAGS) $(KERNEL)/interrupts.s -o $@

setup.o: $(BOOT)/setup.s
	$(AS)  $(ASFLAGS)  $(BOOT)/setup.s -o $@

kernel.o: $(KERNEL)/kernel.c
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/kernel.c -o $@

hello.o: $(KERNEL)/hello.c
	$(CC) $(CFLAGS)$(INCLUDE) $(KERNEL)/hello.c -o $@

kernel.b: setup.o $(OBJS)
	ld $^ -Ttext 0x10000 -e _start -N -o $@

kernel.bin: kernel.b
	objcopy -R .note -R .comment -S -O binary $< $@

#kernel.bin: setup.o $(OBJS)
#	ld -T util/link.ld -o $@ setup.o $(OBJS)

kos.fd0: boot.o kernel.bin
	cp boot.o kos.fd0
	dd if=kernel.bin ibs=512 conv=sync >> kos.fd0

disasm: kernel.bin
	ndisasm -u $< | less

clean:
	rm -f *.o kernel.bin kernel.b kos.fd0 *~

boot:
	dd if=kos.fd0 of=/dev/fd0

install:
	dd if=kos.fd0 of=/dev/fd0

.PHONY: all clean disasm boot kernel








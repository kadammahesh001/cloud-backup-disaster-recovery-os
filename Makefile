CC = i686-elf-gcc
AS = nasm
LD = i686-elf-ld
OBJCOPY = i686-elf-objcopy

CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS = -T linker.ld -nostdlib

KERNEL = kernel.bin
ISO = os.iso

OBJS = \
    boot/entry.o \
    src/kernel.o \
    src/vga.o \
    src/keyboard.o \
    src/shell.o \
    src/idt.o \
    src/isr.o \
    src/string.o \
    src/isr_asm.o

all: $(ISO)

$(KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(ISO): $(KERNEL) grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir 2>/dev/null

boot/entry.o: boot/entry.asm
	$(AS) -f elf32 $< -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

src/isr_asm.o: src/isr.asm
	$(AS) -f elf32 $< -o $@

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio

debug: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio -s -S

clean:
	rm -f $(OBJS) $(KERNEL) $(ISO)
	rm -rf isodir

docker-build:
	docker build -t simple-os .

docker-run:
	docker run --rm -it -v ${PWD}:/kernel simple-os make run

test-headless: $(ISO)
	timeout 10 qemu-system-i386 -cdrom $(ISO) -nographic -serial stdio -no-reboot || true

.PHONY: all run debug clean docker-build docker-run test-headless
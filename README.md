# Simple OS

A minimal x86 OS with a basic shell, written in C and Assembly.

## Features

- VGA text mode driver (80x25, color support, scrolling)
- PS/2 keyboard driver with shift support
- IDT/ISR for exceptions and hardware interrupts
- Minimal shell with built-in commands:
  - `help` - Show available commands
  - `clear` - Clear screen
  - `echo` - Print arguments
  - `reboot` - Reboot system
  - `shutdown` - Shutdown system

## Building

### Option 1: Docker (Recommended)

```bash
# Build and run
docker-compose up

# Or manually
docker build -t simple-os .
docker run --rm -it -v ${PWD}:/kernel simple-os make run
```

### Option 2: Native (Linux/WSL)

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt update && sudo apt install build-essential nasm grub-pc-bin grub-common xorriso qemu-system-x86 gcc-i686-linux-gnu

# Build
cd kernel
make

# Run in QEMU
make run
```

### Option 3: GitHub Actions

Push to GitHub - the workflow will build and test automatically. Download the `os.iso` artifact from the workflow run.

## Project Structure

```
kernel/
├── boot/entry.asm      # Multiboot entry point
├── src/
│   ├── kernel.c        # Main entry
│   ├── vga.c           # VGA text driver
│   ├── keyboard.c      # PS/2 keyboard driver
│   ├── idt.c           # Interrupt Descriptor Table
│   ├── isr.c           # Interrupt Service Routines
│   ├── isr.asm         # Assembly ISR stubs
│   ├── string.c        # String utilities
│   └── shell.c         # Shell implementation
├── include/            # Headers
├── linker.ld           # Linker script
├── grub.cfg            # GRUB config
��── Makefile
```

## Testing

```bash
# Headless test (exits after 10s)
make test-headless

# Debug with GDB
make debug
# Then in another terminal: gdb -ex "target remote localhost:1234" -ex "symbol-file kernel/bin"
```

## Architecture

- Target: i686 (32-bit protected mode)
- Bootloader: GRUB Multiboot
- Language: C99 + NASM assembly
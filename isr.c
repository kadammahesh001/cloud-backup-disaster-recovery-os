#include <isr.h>
#include <vga.h>
#include <keyboard.h>
#include <stdint.h>

static void (*interrupt_handlers[256])(registers_t*);

static const char* exception_messages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment not present",
    "Stack segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 FPU error",
    "Alignment check",
    "Machine check",
    "SIMD FPU exception",
    "Virtualization exception",
    "Control protection exception",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"
};

void isr_handler(registers_t* regs) {
    if (regs->int_no < 32) {
        vga_writestring("\nEXCEPTION: ", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        vga_writestring(exception_messages[regs->int_no], vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        vga_writestring("\n", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        for (;;) __asm__ volatile ("hlt");
    }

    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    }
}

void irq_handler(registers_t* regs) {
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }
    outb(0x20, 0x20);

    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    }
}

void register_interrupt_handler(uint8_t n, void (*handler)(registers_t*)) {
    interrupt_handlers[n] = handler;
}

void isr_init(void) {
    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }
    register_interrupt_handler(33, keyboard_handler);
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
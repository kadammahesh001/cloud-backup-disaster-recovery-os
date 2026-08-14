#include <vga.h>
#include <keyboard.h>
#include <idt.h>
#include <isr.h>
#include <shell.h>

void kernel_main(void) {
    vga_init();
    keyboard_init();
    idt_init();
    isr_init();

    __asm__ volatile ("sti");

    shell_init();
    shell_run();
}
#include <vga.h>
#include <stdint.h>

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;
static size_t terminal_row = 0;
static size_t terminal_col = 0;
static uint8_t terminal_color = 0;

void vga_init(void) {
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_col = 0;
    vga_set_cursor(0, 0);
}

void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
    } else if (c == '\r') {
        terminal_col = 0;
    } else if (c == '\b') {
        if (terminal_col > 0) {
            terminal_col--;
            VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(' ', color);
        }
    } else if (c == '\t') {
        terminal_col = (terminal_col + 4) & ~3;
        if (terminal_col >= VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    } else {
        VGA_MEMORY[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(c, color);
        terminal_col++;
        if (terminal_col >= VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    }

    if (terminal_row >= VGA_HEIGHT) {
        vga_scroll();
    }
    vga_set_cursor(terminal_row, terminal_col);
}

void vga_write(const char* data, size_t size, uint8_t color) {
    for (size_t i = 0; i < size; i++) {
        vga_putchar(data[i], color);
    }
}

void vga_writestring(const char* data, uint8_t color) {
    vga_write(data, strlen(data), color);
}

void vga_set_cursor(size_t row, size_t col) {
    uint16_t pos = row * VGA_WIDTH + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    terminal_row = row;
    terminal_col = col;
}

void vga_get_cursor(size_t* row, size_t* col) {
    *row = terminal_row;
    *col = terminal_col;
}

void vga_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
    terminal_col = 0;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
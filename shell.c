#include <shell.h>
#include <vga.h>
#include <keyboard.h>
#include <string.h>
#include <stdint.h>

#define MAX_INPUT 256
#define MAX_ARGS 16

static char input_buffer[MAX_INPUT];
static int input_pos = 0;

typedef struct {
    const char* name;
    const char* description;
    void (*func)(int argc, char* argv[]);
} command_t;

static void cmd_help(int argc, char* argv[]);
static void cmd_clear(int argc, char* argv[]);
static void cmd_echo(int argc, char* argv[]);
static void cmd_reboot(int argc, char* argv[]);
static void cmd_shutdown(int argc, char* argv[]);

static command_t commands[] = {
    {"help", "Show this help", cmd_help},
    {"clear", "Clear the screen", cmd_clear},
    {"echo", "Print arguments", cmd_echo},
    {"reboot", "Reboot the system", cmd_reboot},
    {"shutdown", "Shutdown the system", cmd_shutdown},
    {0, 0, 0}
};

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void shell_init(void) {
    vga_writestring("Simple OS Shell v1.0\n", vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    vga_writestring("Type 'help' for available commands\n\n", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

static void print_prompt(void) {
    vga_writestring("os> ", vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
}

static void cmd_help(int argc, char* argv[]) {
    vga_writestring("Available commands:\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    for (int i = 0; commands[i].name; i++) {
        vga_writestring("  ", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        vga_writestring(commands[i].name, vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
        vga_writestring(" - ", vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
        vga_writestring(commands[i].description, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
        vga_writestring("\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    }
}

static void cmd_clear(int argc, char* argv[]) {
    vga_clear();
}

static void cmd_echo(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        vga_writestring(argv[i], vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
        if (i < argc - 1) vga_writestring(" ", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    }
    vga_writestring("\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
}

static void cmd_reboot(int argc, char* argv[]) {
    vga_writestring("Rebooting...\n", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    uint8_t good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
    for (;;) __asm__ volatile ("hlt");
}

static void cmd_shutdown(int argc, char* argv[]) {
    vga_writestring("Shutting down...\n", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    outb(0x604, 0x2000);
    for (;;) __asm__ volatile ("hlt");
}

static void execute_command(char* input) {
    char* argv[MAX_ARGS];
    int argc = 0;
    char* token = input;

    while (*token && argc < MAX_ARGS) {
        while (*token == ' ') token++;
        if (!*token) break;
        argv[argc++] = token;
        while (*token && *token != ' ') token++;
        if (*token) *token++ = '\0';
    }

    if (argc == 0) return;

    for (int i = 0; commands[i].name; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].func(argc, argv);
            return;
        }
    }

    vga_writestring("Unknown command: ", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    vga_writestring(argv[0], vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    vga_writestring("\n", vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
}

void shell_run(void) {
    print_prompt();

    while (1) {
        if (keyboard_haschar()) {
            char c = keyboard_getchar();

            if (c == '\n') {
                input_buffer[input_pos] = '\0';
                vga_writestring("\n", vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
                if (input_pos > 0) {
                    execute_command(input_buffer);
                }
                input_pos = 0;
                print_prompt();
            } else if (c == '\b') {
                if (input_pos > 0) {
                    input_pos--;
                    vga_putchar('\b', vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
                }
            } else if (c >= 32 && c <= 126 && input_pos < MAX_INPUT - 1) {
                input_buffer[input_pos++] = c;
                vga_putchar(c, vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
            }
        }
    }
}
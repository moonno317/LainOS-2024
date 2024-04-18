#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include "vga.h"



static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

 

terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);} } }

void terminal_setcolor(uint8_t color) {
	terminal_color = color; }
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color); }
 
void terminal_scroll(void) {
  for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t current_index = y * VGA_WIDTH + x;
      const size_t next_line_index = (y + 1) * VGA_WIDTH + x;
      terminal_buffer[current_index] = terminal_buffer[next_line_index]; } }
  const size_t last_row_index = (VGA_HEIGHT - 1) * VGA_WIDTH;
  for (size_t x = 0; x < VGA_WIDTH; x++) {
    terminal_buffer[last_row_index + x] = vga_entry(' ', terminal_color); } }

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_row++;
    terminal_column = 0; } else if (c == '\b') {
	terminal_putentryat(' ', terminal_color, terminal_column--, terminal_row);
        terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        return; } else {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    terminal_column++; }

  if (terminal_column >= VGA_WIDTH) {
    terminal_row++;
    terminal_column = 0; }

  if (terminal_row >= VGA_HEIGHT) {
    terminal_scroll();
    terminal_row = VGA_HEIGHT - 1; } }
 
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]); }
 
void printl(const char* data) {
 terminal_write(data, strlen(data)); }

static inline void outb(uint16_t port, uint8_t val){
  asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) ); }


static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret; }

static const char scanToAscii[] = {
    0,  // No key
    0,  // Escape
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // Scan codes 0x01-0x0E
    '\t', // Tab
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', // Scan codes 0x10-0x1C
    0, // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, // Scan codes 0x1E-0x29
    '\\', // Backslash
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, // Scan codes 0x2C-0x35
    '*', // Keypad *
    0, // Alt
    ' ', // Space
    0, // Caps Lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10
    0, // Num Lock
    0, // Scroll Lock
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.' // Keypad 7-0, ., and Enter 
};

char getScancode() {
    char c = 0;
    outb(0x60, 0x0); do {
    if (inb(0x60) != c) {
    c = inb(0x60);
    if (c > 0)
    return c; } } while (1); }

void kbd() {
    char scancode = getScancode();
    char character = scanToAscii[(int)scancode];
        if (character == 0x1C) {
            printl("\n"); } else if (character == 0x0E) {
	    printl("\b"); }
            terminal_putchar(character); }

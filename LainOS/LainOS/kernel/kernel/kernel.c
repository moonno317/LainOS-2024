#include <stdbool.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include "GDTandIDT.h"
#include "mm.h"
#include <stdatomic.h>
#include "schedule.h"

#define PAGE_SIZE 4096
#define PAGE_DIRECTORY_ENTRIES 1024
#define VIDEO_MEMORY_ADDRESS 0x1c090000
#define COLUMNS 80
#define ROWS 25
// Define the video memory address and screen dimensions
#define VIDEO_MEMORY_ADDRESS 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void draw_line(int x1, int y1, int x2, int y2, uint8_t color) {
    uint16_t* video_memory = (uint16_t*)VIDEO_MEMORY_ADDRESS;

    size_t offset = y1 * SCREEN_WIDTH + x1;

    int dx = x2 - x1;
    int dy = y2 - y1;

    int x_step = (dx > 0) ? 1 : -1;
    int y_step = (dy > 0) ? 1 : -1;

    dx = (dx < 0) ? -dx : dx;
    dy = (dy < 0) ? -dy : dy;

    int error = (dx > dy ? dx : -dy) / 2;

    while (1) {
        video_memory[offset] = ' ' | (color << 8);
        if (x1 == x2 && y1 == y2)
            break;
        int prev_error = error;
        if (prev_error > -dx) {
            error -= dy;
            x1 += x_step; }
        if (prev_error < dy) {
            error += dx;
            y1 += y_step; }
             offset = y1 * SCREEN_WIDTH + x1; } }

uint32_t page_directory[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));
uint32_t first_page_table[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));

void load_page_directory(uint32_t *page_directory_addr) {
  asm volatile ("mov %0, %%cr3" : : "r" (page_directory_addr) : "memory"); }

void enable_paging(void) {
  asm volatile ("mov %%cr0, %%eax\n"
                "orl $0x80000000, %%eax\n"
                "mov %%eax, %%cr0" : : : "eax", "memory"); }

void paging() {
    for (int i = 0; i < PAGE_DIRECTORY_ENTRIES; i++) {
        page_directory[i] = 0x00000002; }

    for (int i = 0; i < PAGE_SIZE; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | 3; }

    page_directory[0] = ((uint32_t)first_page_table) | 3;
    load_page_directory(page_directory);
    enable_paging(); }


void kernel_main(void) {
    init_gdt();
    init_idt();
    paging();
    init_dynamic_mem();
    terminal_initialize();
    unsigned int VGA_COLOR_WHITE = 0;

draw_line(40, 0, 40, 40, VGA_COLOR_WHITE);

    while (1) {
        schedule();
        kbd(); } }

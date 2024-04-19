#include <stdbool.h>
#include <kernel/tty.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include "GDTandIDT.h"
#include <stdatomic.h>
#include "schedule.h"

#define PAGE_SIZE 4096  ddddd
#define PAGE_DIRECTORY_ENTRIES 1024

uint32_t page_directory[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));
uint32_t first_page_table[PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));

void load_page_directory(unsigned int *page_directory_addr) {
  asm volatile ("mov %0, %%cr3" : : "r" (page_directory_addr) : "memory"); }

void enable_paging(void) {
  asm volatile ("mov %%cr0, %%eax\n"
                "orl $0x80000000, %%eax\n"
                "mov %%eax, %%cr0" : : : "eax", "memory"); }

void paging() {
    for (int i = 0; i < PAGE_DIRECTORY_ENTRIES; i++) {
        page_directory[i] = 0x00000002; }

    for (int i = 0; i < PAGE_SIZE; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | 3;  }

    page_directory[0] = ((unsigned int)first_page_table) | 3; 
    load_page_directory(page_directory);
    enable_paging(); }

void kernel_main(void) {
    init_gdt();
    init_idt();
    terminal_initialize();
    paging();
    while (1) {
        schedule();
        kbd(); }
}


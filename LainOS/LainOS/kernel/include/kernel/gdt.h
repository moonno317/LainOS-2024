#pragma once
#include <stdint.h>

struct gdt_ptr{
	uint16_t Limit;  // the size of the segment
	uint32_t Base;   // the location of the segment
}__attribute__((packed));

struct gdt_entry{
	uint16_t Limit0;
	uint16_t Base0;
	uint8_t Base1;
	uint8_t AccessByte;
	uint8_t Flags;
	uint8_t Base2;
} __attribute__((packed));

struct GDT{
	struct gdt_entry null;					// 0x00
	struct gdt_entry kernel_code;		// 0x08
	struct gdt_entry kernel_data;		// 0x10
} __attribute__((packed)) __attribute__((aligned(0x1000)));

void init_gdt();

// To be filled with assembly in gdt_load.S
extern void gdt_load(uint64_t);

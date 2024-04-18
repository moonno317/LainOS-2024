struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt_entries[3];

void init_gdt(void) {
    gdt_entries[0].limit_low = 0;
    gdt_entries[0].base_low = 0;
    gdt_entries[0].base_mid = 0;
    gdt_entries[0].access = 0;
    gdt_entries[0].granularity = 0;
    gdt_entries[0].base_high = 0;

    gdt_entries[1].limit_low = 0xFFFF;
    gdt_entries[1].base_low = 0;
    gdt_entries[1].base_mid = 0;
    gdt_entries[1].access = 0x9A;
    gdt_entries[1].granularity = 0xCF;
    gdt_entries[1].base_high = 0;

    gdt_entries[2].limit_low = 0xFFFF;
    gdt_entries[2].base_low = 0;
    gdt_entries[2].base_mid = 0;
    gdt_entries[2].access = 0x92;
    gdt_entries[2].granularity = 0xCF;
    gdt_entries[2].base_high = 0;

    struct gdt gdt;
    gdt.limit = sizeof(gdt_entries) - 1;
    gdt.base = (uint32_t)&gdt_entries;

    asm volatile("lgdt %0" : : "m" (gdt));

    asm volatile(
        "movw $0x10, %ax\n"
        "movw %ax, %ds\n"
        "movw %ax, %es\n"
        "movw %ax, %fs\n"
        "movw %ax, %gs\n"
        "movw %ax, %ss\n"
        "ljmp $0x08, $reload_cs\n"
        "reload_cs:\n" ); }

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high; } __attribute__((packed));

struct idt {
    uint16_t limit;
    uint32_t base; } __attribute__((packed));

struct idt_entry idt_entries[256];
struct idt idt_ptr;

void syscall_handler(void) {}

void init_idt_entry(int index, uint32_t offset, uint16_t selector, uint8_t type_attr) {
    idt_entries[index].offset_low = offset & 0xFFFF;
    idt_entries[index].offset_high = (offset >> 16) & 0xFFFF;
    idt_entries[index].selector = selector;
    idt_entries[index].zero = 0;
    idt_entries[index].type_attr = type_attr; }

void init_idt(void) {
    idt_ptr.limit = sizeof(idt_entries) - 1;
    idt_ptr.base = (uint32_t)&idt_entries;
    for (int i = 0; i < 256; i++) {
        init_idt_entry(i, 0, 0, 0); }
    init_idt_entry(0x80, (uint32_t)syscall_handler, 0x08, 0x8E);
    asm volatile("lidt %0" : : "m" (idt_ptr)); }

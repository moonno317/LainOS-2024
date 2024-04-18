typedef struct dynamic_mem_node {
    uint32_t size;
    bool used;
    struct dynamic_mem_node *next;
    struct dynamic_mem_node *prev; } dynamic_mem_node_t;

#define NULL_POINTER ((void*)0)
#define DYNAMIC_MEM_TOTAL_SIZE 4096
#define DYNAMIC_MEM_NODE_SIZE sizeof(dynamic_mem_node_t)

static uint8_t dynamic_mem_area[DYNAMIC_MEM_TOTAL_SIZE];
static dynamic_mem_node_t *dynamic_mem_start;

void init_dynamic_mem() {
    dynamic_mem_start = (dynamic_mem_node_t *)dynamic_mem_area;
    dynamic_mem_start->size = DYNAMIC_MEM_TOTAL_SIZE - DYNAMIC_MEM_NODE_SIZE;
    dynamic_mem_start->used = false;
    dynamic_mem_start->next = NULL_POINTER;
    dynamic_mem_start->prev = NULL_POINTER; }

dynamic_mem_node_t* find_best_mem_block(dynamic_mem_node_t *dynamic_mem, size_t size) {
    dynamic_mem_node_t *best_mem_block = NULL_POINTER;
    uint32_t best_mem_block_size = DYNAMIC_MEM_TOTAL_SIZE + 1;

    dynamic_mem_node_t *current_mem_block = dynamic_mem;
    while (current_mem_block) {
        if ((!current_mem_block->used) &&
            (current_mem_block->size >= (size + DYNAMIC_MEM_NODE_SIZE)) &&
            (current_mem_block->size <= best_mem_block_size)) {
            best_mem_block = current_mem_block;
            best_mem_block_size = current_mem_block->size;} current_mem_block = current_mem_block->next; } return best_mem_block; }

void *malloc(size_t size) {
    dynamic_mem_node_t *mem_block = find_best_mem_block(dynamic_mem_start, size);
    if (mem_block == NULL_POINTER) {
        return NULL_POINTER; }

    if (mem_block->size > (size + DYNAMIC_MEM_NODE_SIZE)) {
        dynamic_mem_node_t *new_free_block = (dynamic_mem_node_t *)((uint8_t *)mem_block + DYNAMIC_MEM_NODE_SIZE + size);
        new_free_block->size = mem_block->size - (size + DYNAMIC_MEM_NODE_SIZE);
        new_free_block->used = false;
        new_free_block->next = mem_block->next;
        new_free_block->prev = mem_block;
        if (mem_block->next != NULL_POINTER) {
            mem_block->next->prev = new_free_block; }
        mem_block->next = new_free_block;
        mem_block->size = size; }
    mem_block->used = true;

    return (void *)((uint8_t *)mem_block + DYNAMIC_MEM_NODE_SIZE); }

void free(void *ptr) {
    if (ptr == NULL_POINTER) {
        return; }

    dynamic_mem_node_t *mem_block = (dynamic_mem_node_t *)((uint8_t *)ptr - DYNAMIC_MEM_NODE_SIZE);
    mem_block->used = false;

    if (mem_block->prev != NULL_POINTER && !mem_block->prev->used) {
        mem_block->prev->size += (mem_block->size + DYNAMIC_MEM_NODE_SIZE);
        mem_block->prev->next = mem_block->next;
        if (mem_block->next != NULL_POINTER) {
            mem_block->next->prev = mem_block->prev; }
        mem_block = mem_block->prev;}

    if (mem_block->next != NULL_POINTER && !mem_block->next->used) {
        mem_block->size += (mem_block->next->size + DYNAMIC_MEM_NODE_SIZE);
        mem_block->next = mem_block->next->next;
        if (mem_block->next != NULL_POINTER) {
            mem_block->next->prev = mem_block; } } }

extern void graphics_clear(unsigned char top_color, unsigned char main_color);
extern void graphics_update_navigation(unsigned char msg[]);
extern void graphics_print(unsigned char msg[], unsigned char x, unsigned char y);
extern void graphics_put_char(unsigned char c, unsigned char x, unsigned char y);
extern void graphics_print_hex(unsigned char num, unsigned char x, unsigned char y);
extern void graphics_move_cursor(unsigned char x, unsigned char y);
extern char in(unsigned short port);
extern void out(unsigned short port, unsigned char value);

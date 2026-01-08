#include "memory.h"
#include "mpu.h"
#include "process.h"
#include "uart.h"

static uint8_t heap_area[HEAP_SIZE] __attribute__((aligned(4096)));
static mem_block_t *free_list = NULL;

void os_mem_init(void) 
{
    free_list = (mem_block_t *)heap_area;
    free_list->next = NULL;
    free_list->size = HEAP_SIZE - sizeof(mem_block_t);
    free_list->is_free = 1;
    
    uart_print("Heap initialized: ");
    uart_print_dec(HEAP_SIZE);
    uart_print(" bytes at 0x");
    uart_print_hex32((uint32_t)heap_area);
    uart_print("\r\n");
}

void* os_malloc(size_t size) 
{
    return os_malloc_aligned(size, 8);
}

void* os_malloc_aligned(size_t size, size_t alignment) 
{
    if (alignment < 8) alignment = 8;
    
    /* Validate alignment is power of 2 */
    if ((alignment & (alignment - 1)) != 0) {
        uart_print("ERROR: Alignment not power of 2\r\n");
        return NULL;
    }

    /* Round size up to 8 bytes */
    size = (size + 7) & ~0x07;
    
    /* Validate size */
    if (size == 0 || size > HEAP_SIZE) {
        uart_print("ERROR: Invalid size\r\n");
        return NULL;
    }

    void *ptr = NULL;

    OS_ENTER_CRITICAL();
    
    mem_block_t *current = free_list;
    mem_block_t *prev = NULL;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            /* Calculate aligned address */
            uintptr_t block_start = (uintptr_t)current;
            uintptr_t data_addr = block_start + sizeof(mem_block_t);
            uintptr_t aligned_addr = (data_addr + alignment - 1) & ~(alignment - 1);
            size_t padding = aligned_addr - data_addr;
            
            /* Check if block is large enough for padding + size */
            size_t total_needed = padding + size;
            
            if (current->size >= total_needed) {
                mem_block_t *allocated_block;
                
                if (padding >= sizeof(mem_block_t) + 8) 
                {
                    mem_block_t *padding_block = current;
                    allocated_block = (mem_block_t*)(aligned_addr - sizeof(mem_block_t));
                    
                    /* Setup padding block */
                    padding_block->size = padding - sizeof(mem_block_t);
                    padding_block->is_free = 1;  // Keep as free
                    padding_block->next = allocated_block;
                    
                    /* Setup allocated block */
                    allocated_block->size = current->size - padding;
                    allocated_block->is_free = 0;
                    allocated_block->next = current->next;
                    
                    /* Update free_list if necessary */
                    if (prev == NULL) {
                        free_list = padding_block;
                    }
                    
                    current = allocated_block;
                } else {
                    /* No room for padding block, use current block */
                    if (padding > 0) {
                        /* Skip this block, alignment impossible */
                        prev = current;
                        current = current->next;
                        continue;
                    }
                    allocated_block = current;
                    allocated_block->is_free = 0;
                }
                
                /* Split block if enough space remains */
                if (allocated_block->size > size + sizeof(mem_block_t) + 16) 
                {
                    mem_block_t *remainder = (mem_block_t*)((uint8_t*)allocated_block + 
                                             sizeof(mem_block_t) + size);
                    
                    remainder->size = allocated_block->size - size - sizeof(mem_block_t);
                    remainder->is_free = 1;
                    remainder->next = allocated_block->next;
                    
                    allocated_block->size = size;
                    allocated_block->next = remainder;
                }
                
                ptr = (void*)((uint8_t*)allocated_block + sizeof(mem_block_t));
                break;
            }
        }
        
        prev = current;
        current = current->next;
    }
    
    OS_EXIT_CRITICAL();
    
    if (ptr == NULL) {
        uart_print("ERROR: os_malloc_aligned failed - size=");
        uart_print_dec(size);
        uart_print(" align=");
        uart_print_dec(alignment);
        uart_print("\r\n");
    }
    
    return ptr;
}

void os_free(void *ptr) 
{
    if (ptr == NULL) return;
    uintptr_t ptr_addr = (uintptr_t)ptr;
    uintptr_t heap_start = (uintptr_t)heap_area;
    uintptr_t heap_end = heap_start + HEAP_SIZE;
    
    if (ptr_addr < heap_start || ptr_addr >= heap_end) {
        uart_print("ERROR: Invalid free - ptr out of heap bounds\r\n");
        return;
    }

    OS_ENTER_CRITICAL();

    mem_block_t *block = (mem_block_t*)((uint8_t*)ptr - sizeof(mem_block_t));
    
    if ((uintptr_t)block < heap_start || (uintptr_t)block >= heap_end) 
    {
        uart_print("ERROR: Invalid block header\r\n");
        OS_EXIT_CRITICAL();
        return;
    }
    
    if (block->is_free) {
        uart_print("WARNING: Double free detected\r\n");
        OS_EXIT_CRITICAL();
        return;
    }
    
    block->is_free = 1;

    if (block->next && block->next->is_free) {
        block->size += sizeof(mem_block_t) + block->next->size;
        block->next = block->next->next;
    }
    
    mem_block_t *current = free_list;
    mem_block_t *prev = NULL;
    
    while (current && current != block) {
        prev = current;
        current = current->next;
    }
    
    if (prev && prev->is_free) {
        prev->size += sizeof(mem_block_t) + block->size;
        prev->next = block->next;
    }
    
    OS_EXIT_CRITICAL();
}

void os_heap_dump(void)
{
    uart_print("\r\n=== Heap Status ===\r\n");
    
    uint32_t total_free = 0;
    uint32_t total_used = 0;
    uint32_t block_count = 0;
    
    OS_ENTER_CRITICAL();
    
    mem_block_t *current = free_list;
    while (current) {
        uart_print("Block ");
        uart_print_dec(block_count++);
        uart_print(": 0x");
        uart_print_hex32((uint32_t)current);
        uart_print(" Size=");
        uart_print_dec(current->size);
        
        if (current->is_free) {
            uart_print(" [FREE]\r\n");
            total_free += current->size;
        } else {
            uart_print(" [USED]\r\n");
            total_used += current->size;
        }
        
        current = current->next;
    }
    
    OS_EXIT_CRITICAL();
    
    uart_print("\r\nTotal Free: ");
    uart_print_dec(total_free);
    uart_print(" bytes\r\n");
    
    uart_print("Total Used: ");
    uart_print_dec(total_used);
    uart_print(" bytes\r\n");
    
    uart_print("Overhead: ");
    uart_print_dec(block_count * sizeof(mem_block_t));
    uart_print(" bytes (");
    uart_print_dec(block_count);
    uart_print(" blocks)\r\n");
    
    uart_print("==================\r\n\r\n");
}

uint32_t mpu_calc_alignment(size_t size) 
{
    uint32_t size_bits = mpu_calc_region_size(size);
    return 1U << (size_bits + 1);
}
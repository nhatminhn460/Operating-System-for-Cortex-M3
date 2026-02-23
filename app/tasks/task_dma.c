#include "task.h"
#include "dma.h"
#include "uart.h"

static char src_buffer[] = "Hello DMA! This string is copied by Hardware.";
static char dst_buffer[64];

/* Hàm delay phụ trợ */
static void delay_loop(volatile uint32_t count) {
    while(count--) { __asm("nop"); }
}

void task_dma_test(void) {
    uart_print("[DMA] Task Started.\n");
    dma_init();
    
    // Xóa buffer đích
    for(int i=0; i<64; i++) dst_buffer[i] = 0;

    while(1) {
        // Copy 46 bytes từ src sang dst
        bool res = dma_memcpy(src_buffer, dst_buffer, sizeof(src_buffer));
        
        if (res) {
            uart_print("[DMA] Copy Success: ");
            uart_print(dst_buffer);
            uart_print("\n");
        } else {
            uart_print("[DMA] Copy Failed.\n");
        }

        delay_loop(25000000); 
    }
}
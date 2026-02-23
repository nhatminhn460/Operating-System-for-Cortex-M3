#include "uart.h"
#include "systick.h"
#include "kernel.h"
#include "task.h"       // Chứa prototype các task và extern
#include "sync.h" 
#include "mpu.h"
#include "ipc.h"
#include <stdint.h>
#include <stddef.h>     // Để dùng NULL
#include "gpio.h"

/* ================================================= */
/* CẤU HÌNH HỆ THỐNG                                 */
/* ================================================= */
#define SYSTEM_CLOCK      50000000 // Clock MCU (50MHz)
#define SYSTICK_RATE      10000000 // Reload value để tạo ngắt mỗi 0.2s (hoặc chỉnh theo ý muốn)

/* ================================================= */
/* ĐỊNH NGHĨA TÀI NGUYÊN TOÀN CỤC (GLOBAL RESOURCES) */
/* ================================================= */
/* BẮT BUỘC: Phải định nghĩa các biến này ở đây để cấp phát RAM.
   File task.h chỉ khai báo extern (cho biết sự tồn tại), 
   còn ở đây mới là nơi tạo ra chúng thực sự. */

// 1. Biến trạng thái hệ thống


/* ================================================= */
/* DỮ LIỆU RIÊNG CHO CÁC TASK                        */
/* ================================================= */
// Tài nguyên yêu cầu tối đa cho Banker's Algorithm
// Nên để toàn cục để đảm bảo dữ liệu không bị mất
int max_res_t1[] = {0, 0, 2}; 
int max_res_t2[] = {0, 0, 2};

/* ================================================= */
/* MAIN FUNCTION                                     */
/* ================================================= */

/* Hàm delay đơn giản */
void delay(volatile unsigned int count) {
    while (count--) {
        __asm("nop");
    }
}

int main(void) {
    /* 1. Khởi tạo phần cứng cơ bản */
    // bsp_init_system_clock(); // Bỏ comment nếu cần cấu hình PLL
    uart_init();
    
    /* 2. Khởi tạo Kernel */
    os_kernel_init();

    /* 3. Khởi tạo Tài nguyên (IPC & Sync) */
    msg_queue_init(&temp_queue);
    mutex_init(&app_mutex);
    mutex_init(&mutex_A);
    mutex_init(&mutex_B);

    /* 4. In thông báo khởi động */
    uart_print("\033[2J"); // Xóa màn hình terminal
    uart_print("MyOS IoT System Booting...\r\n");
    
    /* 5. Tạo các Task (Process) */
    
    // --- Nhóm IoT & System ---
    /*process_create(task_sensor_update, 1, 4, NULL); 
    process_create(task_display,       2, 2, NULL);       
    process_create(task_alarm,         3, 3, NULL);         
    process_create(task_logger,        4, 4, NULL);              
    process_create(task_shell,         5, 1, NULL);*/

    // --- Nhóm Test Deadlock ---
    process_create(task_deadlock_1,    6, 5, NULL);
    process_create(task_deadlock_2,    7, 5, NULL);

    // --- Nhóm Test Banker's Algorithm ---
    //process_create(task_banker1,       8, 4, max_res_t1);
    //process_create(task_banker2,       9, 4, max_res_t2);

    // --- Nhóm Test Drivers ---
    // process_create(task_gpio_blink, 10, 5, NULL); // Task nháy đèn (Tạm tắt)
    //process_create(task_i2c_scanner,   10, 5, NULL); // Task quét I2C (Mới thêm)
    //process_create(task_dma_test, 10, 5, NULL); // Task test DMA

    /* 6. Khởi động System Tick (Nhịp tim hệ điều hành) */
    // Khi hàm này chạy, Scheduler sẽ bắt đầu hoạt động
    systick_init(SYSTICK_RATE); 

    /* 7. Vòng lặp Idle */
    while (1) {
        // CPU sẽ chạy vào đây khi không có task nào khác hoạt động
        // Có thể đưa CPU vào chế độ ngủ (Sleep mode) để tiết kiệm điện
        // __asm("wfi"); // Wait For Interrupt
    }
}
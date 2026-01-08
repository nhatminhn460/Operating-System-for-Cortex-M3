#include "uart.h"
#include "systick.h"
#include "process.h"
#include "task.h"
#include "sync.h" 
#include "ipc.h"
#include "mpu.h"
#include <stdint.h>


#define SYSTEM_CLOCK      80000000 // clock mcu 
#define SYSTICK_RATE      8000000  // set systick reload để tạo ngắt mỗi 0.1s (10Hz)
// nhịp tim của hệ điều hành, nó sẽ đếm từ  8 000 000 về 0

os_msg_queue_t temp_queue; // Hàng đợi tin nhắn cho nhiệt độ
os_mutex_t app_mutex; // chiếc khóa chung cho cả hệ thống

// tạo deadlock giả
os_mutex_t mutex_A;
os_mutex_t mutex_B;
// tạo deadlock giả

void delay(volatile unsigned int count) {
    while (count--) {
        __asm("nop");
    }
}

void task0(void) 
{
    for(int i = 0; i < 100000; i++)
    {
        uart_print_dec(i);
        uart_print(" ");
        if (i % 100 == 0) 
        {
            os_delay(1);
        }
    }
    while(1) 
    {
        uart_print("[Task0] Done!\r\n");
        os_delay(100);
    }
}

void task1(void) 
{
    for(int i = 100000; i > 0; i--)
    {
        uart_print_dec(i);
        uart_print(" ");
        if (i % 100 == 0) 
        {
            os_delay(1);
        }
    }
    
    while(1) 
    {
        uart_print("[Task1] Done!\r\n");
        os_delay(100);
    }
}
/* --- MAIN --- */
void main(void) {
    uart_init();
    banker_init();
    mpu_init();
    process_init();
    msg_queue_init(&temp_queue);
    mutex_init(&app_mutex);
    mutex_init(&mutex_A);
    mutex_init(&mutex_B);

    int max_res_t1[] = {0, 0, 2}; 
    int max_res_t2[] = {0, 0, 2};

    uart_print("\033[2J"); // Lệnh xóa màn hình terminal (nếu hỗ trợ)
    uart_print("MyOS IoT System Booting...\r\n");
    delay(5000000); // Chờ khởi động

    /* Tạo các task với chức năng cụ thể */
    /*process_create(task_sensor_update, 1, 4, NULL); 
    process_create(task_display, 2, 2, NULL);       
    process_create(task_alarm, 3, 3, NULL);         
    process_create(task_logger, 4, 4, NULL);              
    process_create(task_shell, 5, 1, NULL);
    process_create(task_deadlock_1, 6, 5, NULL);
    process_create(task_deadlock_2, 7, 5, NULL);
    process_create(task_banker1, 8, 4, max_res_t1);
    process_create(task_banker2, 9, 4, max_res_t2);
    process_create(test_mpu_fault, 10, 6, NULL);*/
    //process_admit_jobs();
    process_create(task0, 1, 3, NULL);
    process_create(task1, 2, 4, NULL);
    process_create(test_mpu_fault, 3, 6, NULL);
    /* Khởi động nhịp tim hệ thống */
    systick_init(SYSTICK_RATE); // kích hoạt hệ thống 

    while (1) {
        // Idle task: Có thể dùng để tính toán uptime hoặc ngủ tiết kiệm điện
        // Ở đây ta để trống để nhường CPU cho các task kia
    }
}
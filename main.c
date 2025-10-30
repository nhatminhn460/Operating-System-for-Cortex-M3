#include "uart.h"
#include "systick.h"
#include "process.h"

void task1(void) {
    while (1) {
        uart_print("Task 1 running...\r\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void task2(void) {
    while (1) {
        uart_print("Task 2 running...\r\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void main(void) {
    uart_init();
    uart_print("Hello MyOS\r\n");

    process_init();
    process_create(task1, 0);
    process_create(task2, 1);

    systick_init(2000000);

    while (1) {
        uart_print("Idle process...\r\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

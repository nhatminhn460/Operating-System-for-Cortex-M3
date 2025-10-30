.syntax unified
.cpu cortex-m3
.thumb

.section .isr_vector, "a", %progbits
.global _estack
.global Reset_Handler

/* Vector Table */
vector_table:
    .word _estack            /* Initial Stack Pointer */
    .word Reset_Handler      /* Reset Handler */
    .word Default_Handler     /* NMI */
    .word Default_Handler     /* HardFault */
    .word Default_Handler     /* MemManage */
    .word Default_Handler     /* BusFault */
    .word Default_Handler     /* UsageFault */
    .word 0,0,0,0             /* Reserved */
    .word Default_Handler     /* SVC */
    .word Default_Handler     /* DebugMon */
    .word 0
    .word PendSV_Handler     /* PendSV */
    .word SysTick_Handler     /* SysTick Handler (định nghĩa trong C) */

    /* IRQs (không dùng) */
    .rept 32
        .word Default_Handler
    .endr

.text
.thumb_func
Reset_Handler:
    bl main
    b .

.thumb_func
PendSV_Handler:
    push {lr}
    bl process_schedule
    pop {lr}
    bx lr

.thumb_func
Default_Handler:
    b .

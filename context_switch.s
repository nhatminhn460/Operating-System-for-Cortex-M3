.syntax unified
.thumb
.cpu cortex-m3

.global switch_context
.global start_first_task

// void switch_context(uint32_t **old_sp_ptr, uint32_t *new_sp)
// r0 = &old_sp_location, r1 = new_sp_value 
.thumb_func
switch_context:
    mrs r2, psp            // Lấy PSP hiện tại
    stmdb r2!, {r4-r11}    // Lưu context (R4-R11)
    str r2, [r0]           // Lưu SP hiện tại vào *old_sp

    mov r2, r1             // r1 chứa SP mới
    ldmia r2!, {r4-r11}    // Phục hồi context (R4-R11)
    msr psp, r2            // Nạp PSP mới
    bx lr

// void start_first_task(uint32_t *first_sp)
// r0 = pointer to R4 region of first task's stack
.thumb_func
start_first_task:
    mov    r2, r0             // r2 = first_sp (pointer to R4)
    ldmia  r2!, {r4-r11}      // restore R4-R11 from first stack
    msr    psp, r2            // PSP now points to R0 (exception frame)
    movs   r0, #2
    msr    CONTROL, r0        // switch to Thread mode, use PSP
    isb
    ldr    lr, =0xFFFFFFFD    // set EXC_RETURN into LR
    bx     lr                 // perform exception return (pop R0..xPSR and start task)

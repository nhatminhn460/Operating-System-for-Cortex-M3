#include "mpu.h"
#include "uart.h"
#define SCB_ICSR (*(volatile uint32_t*)0xE000ED04)
#define PENDSVSET_BIT (1UL << 28)

void mpu_init(void)
{
    uart_print("Initializing MPU for QEMU...\r\n");
    
    MPU_CTRL = 0;
    __DSB();

    /* REGION 0: Flash */
    MPU_RNR  = 0;
    MPU_RBAR = 0x00000000;
    MPU_RASR =
        (0 << MPU_RASR_XN_Pos) |
        (2 << MPU_RASR_AP_Pos) |
        (0 << MPU_RASR_TEX_Pos) |
        (1 << MPU_RASR_C_Pos) |
        (0 << MPU_RASR_B_Pos) |
        (0 << MPU_RASR_S_Pos) |
        (17 << MPU_RASR_SIZE_Pos) |
        (1 << MPU_RASR_ENABLE_Pos);
    
    uart_print("  Region 0 (Flash): 0x00000000, 256KB\r\n");

    /* REGION 2: RAM .data/.bss (SHARED BY ALL TASKS) */
    MPU_RNR  = 2;
    MPU_RBAR = 0x20000000;  /* RAM base */
    MPU_RASR =
        (1 << MPU_RASR_XN_Pos) |      /* No Execute */
        (3 << MPU_RASR_AP_Pos) |      /* Full Access */
        (1 << MPU_RASR_TEX_Pos) |     /* Normal memory */
        (1 << MPU_RASR_C_Pos) |
        (1 << MPU_RASR_B_Pos) |
        (0 << MPU_RASR_S_Pos) |
        (15 << MPU_RASR_SIZE_Pos) |   /* 64KB (entire RAM) */
        (1 << MPU_RASR_ENABLE_Pos);
    
    uart_print("  Region 2 (RAM): 0x20000000, 64KB\r\n");

    /* REGION 3: Peripherals */
    MPU_RNR  = 3;
    MPU_RBAR = 0x40000000;
    MPU_RASR =
        (1 << MPU_RASR_XN_Pos) |
        (3 << MPU_RASR_AP_Pos) |
        (0 << MPU_RASR_TEX_Pos) |
        (0 << MPU_RASR_C_Pos) |
        (1 << MPU_RASR_B_Pos) |
        (1 << MPU_RASR_S_Pos) |
        (28 << MPU_RASR_SIZE_Pos) |
        (1 << MPU_RASR_ENABLE_Pos);
    
    uart_print("  Region 3 (Peripherals): 0x40000000, 512MB\r\n");

    /* REGION 4: System */
    MPU_RNR  = 4;
    MPU_RBAR = 0xE0000000;
    MPU_RASR =
        (1 << MPU_RASR_XN_Pos) |
        (3 << MPU_RASR_AP_Pos) |
        (0 << MPU_RASR_TEX_Pos) |
        (0 << MPU_RASR_C_Pos) |
        (1 << MPU_RASR_B_Pos) |
        (1 << MPU_RASR_S_Pos) |
        (28 << MPU_RASR_SIZE_Pos) |
        (1 << MPU_RASR_ENABLE_Pos);
    
    uart_print("  Region 4 (System): 0xE0000000, 512MB\r\n");

    /* REGION 5: Flash mirror (optional) */
    MPU_RNR  = 5;
    MPU_RBAR = 0x08000000;
    MPU_RASR =
        (0 << MPU_RASR_XN_Pos) |
        (2 << MPU_RASR_AP_Pos) |
        (0 << MPU_RASR_TEX_Pos) |
        (1 << MPU_RASR_C_Pos) |
        (0 << MPU_RASR_B_Pos) |
        (0 << MPU_RASR_S_Pos) |
        (17 << MPU_RASR_SIZE_Pos) |
        (1 << MPU_RASR_ENABLE_Pos);
    
    uart_print("  Region 5 (Flash mirror): 0x08000000, 256KB\r\n");

    SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    MPU_CTRL = MPU_CTRL_ENABLE_Msk | MPU_CTRL_PRIVDEFENA_Msk;
    
    __DSB();
    __ISB();
    
    uart_print("MPU enabled\r\n");
}

void mpu_config_for_task(PCB_t *task)
{
    if (!task) return;
    
    /* MPU Region 2 đã cover toàn bộ RAM → Không cần config lại */
    /* Chỉ cần update nếu muốn isolate stack giữa các task */
    
    return;  /* Tạm thời disable per-task MPU */
}

void MemManage_Handler(void)
{
    uart_print("\r\n*** MPU FAULT ***\r\n");

    if (current_pcb)
    {
        uart_print("Task ID: ");
        uart_print_dec(current_pcb->pid);
        uart_print("\r\n");
    }

    uint8_t mmfsr = SCB_CFSR & 0xFF;
    uart_print("MMFSR: 0x");
    uart_print_hex(mmfsr);
    uart_print("\r\n");

    if (mmfsr & (1 << 7))  // MMARVALID
    {
        uint32_t fault_addr = SCB_MMFAR;
        uart_print("Fault Addr: 0x");
        uart_print_hex32(fault_addr);
        uart_print("\r\n");
    }

    /* Clear fault flags */
    SCB_CFSR |= 0xFF;

    /* Suspend faulting task */
    if (current_pcb)
    {
        current_pcb->state = PROC_SUSPENDED;
        uart_print("Task suspended\r\n");
        current_pcb = NULL;
    }

    SCB_ICSR |= PENDSVSET_BIT;    
    return;
}
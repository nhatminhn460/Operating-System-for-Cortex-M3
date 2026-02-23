.syntax unified // GNU(trình biên dịch assembly) bật UAL(cú pháp thống nhất do arm đưa ra)
.cpu cortex-m3 // tên chip là cortex m3 -> cho biết đúng kiến trúc cpu để kiểm tra tập lệnh, encoding và tối ưu mã máy
.thumb //  sử dụng tập lệnh thumb (16 bit) 

/* ========================================
   PHẦN 1: KHAI BÁO GLOBAL & EXTERN
   ======================================== */
   // global là được dùng ở file khác có thể truy cập được, được linker sử dụng để liên kết các tệp khác nhau lại với nhau
   // nếu không khai báo global thì linker không tùn thấy thì vector table không được đưa vào đúng địa chỉ
.global vector_table // Định nghĩa vector_table là một biểu tượng toàn cục
.global Reset_Handler // Định nghĩa Reset_Handler là một biểu tượng toàn cục

.extern _estack // đỉnh của stack - thường là cuối RAM
.extern _sidata   /* Đầu dữ liệu trong Flash */
.extern _sdata    /* Đầu dữ liệu trong RAM */
.extern _edata    /* Cuối dữ liệu trong RAM */
.extern _sbss     /* Đầu vùng BSS */
.extern _ebss     /* Cuối vùng BSS */

.extern main  // nói với assembly rằng hàm main() được định nghĩa ở C, rết handler sẽ gọi bl main
.extern PendSV_Handler
.extern SysTick_Handler
.extern start_first_task // hàm khởi tạo task đầu tiên trong hệ điều hành thời gian thực

/* ========================================
   PHẦN 2: VECTOR TABLE
   ======================================== */
.section .isr_vector, "a", %progbits // đưa phần mã tiếp theo vào section tên .isr_vector, với thuộc tính "a" (allocatable -> section này sẽ được đưa vào falsh bởi linker) và loại %progbits (dữ liệu chương trình)
.type vector_table, %object // định nghĩa vector_table là một đối tượng (object)

vector_table:

/* ===============================
 * CORE EXCEPTIONS (0–15)
 * =============================== */
.word _estack                  /* 0  Initial MSP */
.word Reset_Handler            /* 1  Reset */
.word NMI_Handler              /* 2  NMI */
.word HardFault_Handler        /* 3  HardFault */
.word MemManage_Handler        /* 4  MemManage */
.word BusFault_Handler         /* 5  BusFault */
.word UsageFault_Handler       /* 6  UsageFault */
.word 0                        /* 7  Reserved */
.word 0                        /* 8  Reserved */
.word 0                        /* 9  Reserved */
.word 0                        /* 10 Reserved */
.word SVC_Handler              /* 11 SVC */
.word DebugMon_Handler         /* 12 Debug Monitor */
.word 0                        /* 13 Reserved */
.word PendSV_Handler           /* 14 PendSV */
.word SysTick_Handler          /* 15 SysTick */

/* ===============================
 * EXTERNAL INTERRUPTS (IRQ0–IRQ42)
 * LM3S6965 – Datasheet order
 * =============================== */

/* GPIO */
.word GPIOA_Handler             /* IRQ0  GPIO Port A */ 
.word GPIOB_Handler             /* IRQ1  GPIO Port B */
.word GPIOC_Handler             /* IRQ2  GPIO Port C */
.word GPIOD_Handler             /* IRQ3  GPIO Port D */
.word GPIOE_Handler             /* IRQ4  GPIO Port E */

/* UART */
.word UART0_Handler             /* IRQ5  UART0 */
.word UART1_Handler             /* IRQ6  UART1 */

/* SSI / SPI */
.word SSI0_Handler              /* IRQ7  SSI0 */
.word SSI1_Handler              /* IRQ8  SSI1 */

/* I2C */
.word I2C0_Handler              /* IRQ9  I2C0 */

/* PWM / Fault */
.word PWM0_FAULT_Handler        /* IRQ10 PWM Fault */

/* PWM Generators */
.word PWM0_GEN0_Handler         /* IRQ11 PWM Generator 0 */
.word PWM0_GEN1_Handler         /* IRQ12 PWM Generator 1 */
.word PWM0_GEN2_Handler         /* IRQ13 PWM Generator 2 */

/* Quadrature Encoder */
.word QEI0_Handler              /* IRQ14 QEI0 */

/* ADC */
.word ADC0_SEQ0_Handler         /* IRQ15 ADC Sequence 0 */
.word ADC0_SEQ1_Handler         /* IRQ16 ADC Sequence 1 */
.word ADC0_SEQ2_Handler         /* IRQ17 ADC Sequence 2 */
.word ADC0_SEQ3_Handler         /* IRQ18 ADC Sequence 3 */

/* Watchdog */
.word WATCHDOG_Handler          /* IRQ19 Watchdog Timer */

/* Timers */
.word TIMER0A_Handler           /* IRQ20 Timer 0A */
.word TIMER0B_Handler           /* IRQ21 Timer 0B */
.word TIMER1A_Handler           /* IRQ22 Timer 1A */
.word TIMER1B_Handler           /* IRQ23 Timer 1B */
.word TIMER2A_Handler           /* IRQ24 Timer 2A */
.word TIMER2B_Handler           /* IRQ25 Timer 2B */
.word TIMER3A_Handler           /* IRQ26 Timer 3A */
.word TIMER3B_Handler           /* IRQ27 Timer 3B */

/* Analog Comparator */
.word COMP0_Handler             /* IRQ28 Analog Comparator 0 */
.word COMP1_Handler             /* IRQ29 Analog Comparator 1 */

/* System Control */
.word SYSCTL_Handler            /* IRQ30 System Control */

/* Flash / EEPROM */
.word FLASH_Handler             /* IRQ31 Flash Control */

/* GPIO F */
.word GPIOF_Handler             /* IRQ32 GPIO Port F */

/* Ethernet */
.word ETH_Handler               /* IRQ33 Ethernet */

/* Hibernation */
.word HIBERNATE_Handler         /* IRQ34 Hibernate Module */

/* USB */
.word USB0_Handler              /* IRQ35 USB0 */

/* PWM Generator 3 */
.word PWM0_GEN3_Handler         /* IRQ36 PWM Generator 3 */

/* uDMA */
.word UDMA_Handler              /* IRQ37 uDMA */

/* uDMA Error */
.word UDMA_ERR_Handler          /* IRQ38 uDMA Error */

/* ADC1 */
.word ADC1_SEQ0_Handler         /* IRQ39 ADC1 Sequence 0 */
.word ADC1_SEQ1_Handler         /* IRQ40 ADC1 Sequence 1 */
.word ADC1_SEQ2_Handler         /* IRQ41 ADC1 Sequence 2 */
.word ADC1_SEQ3_Handler         /* IRQ42 ADC1 Sequence 3 */

/* ========================================
   PHẦN 3: RESET HANDLER : copy data từ flash sang ram , xóa vùng bss và gọi main
   ======================================== */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    /* 1. Copy .data từ Flash sang RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b loop_copy_data

copy_data:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

loop_copy_data:
    adds r4, r0, r3
    cmp r4, r1
    bcc copy_data

    /* 2. Xóa .bss về 0 */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b loop_zero_bss

zero_bss:
    str r3, [r2]
    adds r2, r2, #4

loop_zero_bss:
    cmp r2, r4
    bcc zero_bss

    /* 3. Vào Main */
    bl main
    b .

/* Default Handler */
.section .text.Default_Handler
.weak Default_Handler
.type Default_Handler, %function
Default_Handler: // phần interrupt chưa được đinh nghĩa
    b .
    
/* ===================================================
   PHẦN 4: WEAK ALIAS DEFINITIONS
   =================================================== */

   /* Macro giúp viết code ngắn gọn hơn */
    .macro def_irq_handler handler_name
    .weak \handler_name
    .set  \handler_name, Default_Handler
    .endm

    /* Core Exceptions */
    def_irq_handler NMI_Handler
    def_irq_handler HardFault_Handler
    def_irq_handler MemManage_Handler
    def_irq_handler BusFault_Handler
    def_irq_handler UsageFault_Handler
    def_irq_handler SVC_Handler
    def_irq_handler DebugMon_Handler
    def_irq_handler PendSV_Handler
    def_irq_handler SysTick_Handler

    /* External Interrupts (LM3S6965) */
    def_irq_handler GPIOA_Handler
    def_irq_handler GPIOB_Handler
    def_irq_handler GPIOC_Handler
    def_irq_handler GPIOD_Handler
    def_irq_handler GPIOE_Handler
    def_irq_handler UART0_Handler
    def_irq_handler UART1_Handler
    def_irq_handler SSI0_Handler
    def_irq_handler SSI1_Handler
    def_irq_handler I2C0_Handler
    def_irq_handler PWM0_FAULT_Handler
    def_irq_handler PWM0_GEN0_Handler
    def_irq_handler PWM0_GEN1_Handler
    def_irq_handler PWM0_GEN2_Handler
    def_irq_handler PWM0_GEN3_Handler
    def_irq_handler QEI0_Handler
    def_irq_handler ADC0_SEQ0_Handler
    def_irq_handler ADC0_SEQ1_Handler
    def_irq_handler ADC0_SEQ2_Handler
    def_irq_handler ADC0_SEQ3_Handler
    def_irq_handler WATCHDOG_Handler
    def_irq_handler TIMER0A_Handler
    def_irq_handler TIMER0B_Handler
    def_irq_handler TIMER1A_Handler
    def_irq_handler TIMER1B_Handler
    def_irq_handler TIMER2A_Handler
    def_irq_handler TIMER2B_Handler
    def_irq_handler TIMER3A_Handler
    def_irq_handler TIMER3B_Handler
    def_irq_handler COMP0_Handler
    def_irq_handler COMP1_Handler
    def_irq_handler SYSCTL_Handler
    def_irq_handler FLASH_Handler
    def_irq_handler GPIOF_Handler
    def_irq_handler ETH_Handler
    def_irq_handler HIBERNATE_Handler
    def_irq_handler USB0_Handler
    def_irq_handler UDMA_Handler
    def_irq_handler UDMA_ERR_Handler
    def_irq_handler ADC1_SEQ0_Handler
    def_irq_handler ADC1_SEQ1_Handler
    def_irq_handler ADC1_SEQ2_Handler
    def_irq_handler ADC1_SEQ3_Handler
    
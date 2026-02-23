/* Điểm vào của chương trình là Reset_Handler */
ENTRY(Reset_Handler)

/* Khai báo vùng nhớ cho LM3S6965 */
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}

SECTIONS
{
    /* 1. Code và hằng số nằm trong FLASH */
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))  /* Giữ lại bảng vector ngắt ở đầu Flash */
        *(.text*)             /* Code chương trình (.text) */
        *(.rodata*)           /* Dữ liệu chỉ đọc (.rodata) */
        . = ALIGN(4);
        _etext = .;           /* Đánh dấu cuối vùng text */
    } > FLASH

    /* 2. Dữ liệu khởi tạo (.data) copy từ FLASH sang RAM */
    /* AT(_etext) nghĩa là: Lưu ở Flash (ngay sau code), nhưng chạy ở RAM */
    .data : AT(_etext)
    {
        _sdata = .;           /* Bắt đầu vùng data trong RAM */
        *(.data*)
        . = ALIGN(4);
        _edata = .;           /* Kết thúc vùng data trong RAM */
    } > RAM

    /* Định nghĩa biến để startup.s biết copy từ đâu */
    _sidata = LOADADDR(.data);

    /* 3. Dữ liệu chưa khởi tạo (.bss) nằm trong RAM (khởi tạo về 0) */
    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM

    /* 4. Định nghĩa đỉnh Stack (Cuối RAM) */
    _estack = ORIGIN(RAM) + LENGTH(RAM);
}
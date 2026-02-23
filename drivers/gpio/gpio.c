#include "gpio.h"
#include <stdint.h>

/* --- HÀM NỘI BỘ (Private Helper) --- */
/* Hàm này tìm bit mask tương ứng trong thanh ghi RCGC2 dựa vào Port Base */
static uint32_t get_rcgc2_mask(uint32_t port_base) {
    switch (port_base) {
        case GPIO_PORTA_BASE: return 0x01; // Bit 0
        case GPIO_PORTB_BASE: return 0x02; // Bit 1
        case GPIO_PORTC_BASE: return 0x04; // Bit 2
        case GPIO_PORTD_BASE: return 0x08; // Bit 3
        case GPIO_PORTE_BASE: return 0x10; // Bit 4
        case GPIO_PORTF_BASE: return 0x20; // Bit 5
        case GPIO_PORTG_BASE: return 0x40; // Bit 6
        default: return 0;
    }
}

/* --- PUBLIC FUNCTIONS --- */

void gpio_init(uint32_t port_base, uint8_t pin_mask, uint8_t direction) {
    // 1. Bật Clock cho Port (RCGC2)
    uint32_t rcgc_mask = get_rcgc2_mask(port_base);
    
    // Kích hoạt bit tương ứng trong SYSCTL_RCGC2
    // Lưu ý: SYSCTL_RCGC2 đã được define trong gpio.h là con trỏ volatile
    SYSCTL_RCGC2 |= rcgc_mask;

    // Đợi một chút để clock ổn định (cần ít nhất 3 chu kỳ clock)
    volatile uint32_t delay = SYSCTL_RCGC2; 
    (void)delay; // Tránh warning "unused variable"

    // 2. Cho phép ghi vào các thanh ghi (chỉ cần thiết cho GPIO lock như Port F0, D7)
    // Nếu dùng Port F pin 0 (SW2 trên board TI), cần mở khóa CR. 
    // Ở mức cơ bản ta có thể bỏ qua hoặc thêm vào sau.

    // 3. Tắt tính năng Analog (AMSEL) - Offset 0x528 (Mặc định 0 nên có thể bỏ qua nếu code tối giản)
    
    // 4. Chọn chức năng GPIO (PCTL) - Mặc định 0

    // 5. Set hướng (DIR)
    /* Tính địa chỉ thanh ghi DIR: Base + Offset */
    volatile uint32_t *gpio_dir = (uint32_t *)(port_base + GPIO_DIR_OFFSET);
    
    if (direction == GPIO_DIR_OUTPUT) {
        *gpio_dir |= pin_mask; // Set bit thành 1 (Output)
    } else {
        *gpio_dir &= ~pin_mask; // Clear bit thành 0 (Input)
    }

    // 6. Tắt chức năng thay thế (AFSEL)
    volatile uint32_t *gpio_afsel = (uint32_t *)(port_base + GPIO_AFSEL_OFFSET);
    *gpio_afsel &= ~pin_mask;

    // 7. Bật Digital Enable (DEN)
    volatile uint32_t *gpio_den = (uint32_t *)(port_base + GPIO_DEN_OFFSET);
    *gpio_den |= pin_mask;
}

void gpio_write(uint32_t port_base, uint8_t pin_mask, uint8_t value) {
    // Ghi dữ liệu: Base + (Mask << 2). Kỹ thuật bit-banding của ARM Cortex-M
    // Tuy nhiên cách đơn giản nhất là ghi vào offset DATA + (pin_mask << 2) để chỉ ảnh hưởng pin đó
    // Hoặc ghi thẳng vào DATA_OFFSET (0x3FC) nếu muốn ghi đè.
    
    // Cách an toàn chuẩn Stellaris/Tiva:
    // Dùng kỹ thuật mask-addr: Dịch pin_mask sang trái 2 bit để làm offset
    volatile uint32_t *gpio_data_masked = (uint32_t *)(port_base + (pin_mask << 2));
    
    if (value) {
        *gpio_data_masked = 0xFF; // Ghi bất kỳ giá trị khác 0 nào vào vị trí mask cũng được
    } else {
        *gpio_data_masked = 0x00;
    }
}

void gpio_toggle(uint32_t port_base, uint8_t pin_mask) {
    volatile uint32_t *gpio_data = (uint32_t *)(port_base + (pin_mask << 2));
    *gpio_data ^= 0xFF; // Đảo bit
}

uint32_t gpio_read(uint32_t port_base, uint8_t pin_mask) {
    volatile uint32_t *gpio_data = (uint32_t *)(port_base + (pin_mask << 2));
    return (*gpio_data != 0) ? 1 : 0;
}
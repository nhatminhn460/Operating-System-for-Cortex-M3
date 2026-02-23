# ====================================================================
# PROJECT CONFIGURATION
# ====================================================================
TARGET = kernel
BUILD_DIR = build

# 1. Toolchain
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# 2. Compiler Flags
CFLAGS  = -mcpu=cortex-m3 -mthumb -O2 -ffreestanding -nostdlib -g -Wall -std=c99

# --- INCLUDE PATHS ---
CFLAGS += -I./include
CFLAGS += -I./kernel/include
CFLAGS += -I./drivers/serial
CFLAGS += -I./drivers/timer
CFLAGS += -I./drivers/gpio
CFLAGS += -I./drivers/i2c
CFLAGS += -I./drivers/dma
CFLAGS += -I./app        
CFLAGS += -I./app/tasks 

# 3. Linker Flags
LDSCRIPT = bsp/lm3s6965/linker.ld
LDFLAGS  = -T $(LDSCRIPT) -nostdlib

# ====================================================================
# SOURCE FILE MAPPING (VPATH & SOURCES)
# ====================================================================

# 4. VPATH: Giúp Make tìm thấy file .c nằm rải rác
VPATH += arch/arm_cm3
VPATH += kernel/core kernel/ipc kernel/mem kernel/algo
VPATH += drivers/serial drivers/timer drivers/gpio drivers/i2c drivers/dma
VPATH += app
VPATH += app/tasks

# 5. Source Files
# [CẬP NHẬT] Đã thêm task_dma.c vào đây
SRCS_C  = main.c app_global.c shell.c sensor.c deadlock.c task_gpio.c task_i2c.c task_dma.c
SRCS_C += syscalls.c scheduler.c task_manage.c timer.c utils.c
SRCS_C += queue.c sync.c ipc.c banker.c heap.c
SRCS_C += uart_lm3s.c systick.c mpu.c gpio.c i2c.c dma.c

SRCS_S  = startup.s context.s

# --- [QUAN TRỌNG] TẠO DANH SÁCH OBJECT TRONG FOLDER BUILD ---
# Bước 1: Đổi đuôi .c/.s thành .o
# Bước 2: Thêm tiền tố "build/" vào trước tên file
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS_C:.c=.o) $(SRCS_S:.s=.o))

# ====================================================================
# BUILD RULES
# ====================================================================

# Mục tiêu mặc định
all: $(BUILD_DIR)/$(TARGET).bin

# Tạo file Binary từ ELF
$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	@echo "Build Success! Output: $@"

# Link tất cả file .o thành file .elf
$(BUILD_DIR)/$(TARGET).elf: $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

# Biên dịch file .c -> .o và đặt vào folder build/
# Lưu ý: Nhờ VPATH, Make tự tìm thấy file .c bất kể nó ở đâu trong list VPATH
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Biên dịch file .s -> .o và đặt vào folder build/
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(BUILD_DIR)
	@echo "Assembling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Chạy QEMU
run:
	qemu-system-arm -M lm3s6965evb -kernel $(BUILD_DIR)/$(TARGET).bin -serial mon:stdio -nographic

# Dọn dẹp
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean
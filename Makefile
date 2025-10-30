TARGET = kernel
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb -O2 -ffreestanding -nostdlib
LDFLAGS = -T linker.ld -nostdlib

SRC = main.c startup.s uart.c systick.c process.c context_switch.s

all: $(TARGET).bin

$(TARGET).elf: $(SRC) linker.ld
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

run:
	qemu-system-arm -M lm3s6965evb -kernel kernel.bin -serial mon:stdio -nographic

clean:
	rm -f $(TARGET).elf $(TARGET).bin

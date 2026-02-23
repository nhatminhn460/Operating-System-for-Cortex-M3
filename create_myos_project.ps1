# ===============================================
# Script tạo cấu trúc MyOS_Project
# Chạy trong PowerShell (Windows)
# ===============================================

# Danh sách thư mục
$folders = @(
    "arch/arm_cm3",
    "bsp/lm3s6965",
    "kernel/core",
    "kernel/ipc",
    "kernel/mem",
    "kernel/algo",
    "drivers/serial",
    "drivers/timer",
    "drivers/gpio",
    "drivers/dma",
    "include",
    "app/tasks",
    "build"
)

foreach ($f in $folders) {
    New-Item -ItemType Directory -Path $f -Force | Out-Null
}

# Danh sách file skeleton
$files = @(
    "Makefile",
    "myos.config",

    "arch/arm_cm3/startup.s",
    "arch/arm_cm3/context.s",
    "arch/arm_cm3/mpu.c",
    "arch/arm_cm3/nvic.c",

    "bsp/lm3s6965/linker.ld",

    "kernel/core/scheduler.c",
    "kernel/core/syscalls.c",
    "kernel/core/kernel.c",

    "kernel/ipc/queue.c",
    "kernel/ipc/sync.c",
    "kernel/ipc/ipc.c",

    "kernel/mem/heap.c",

    "kernel/algo/banker.c",

    "drivers/serial/uart_lm3s.c",
    "drivers/timer/systick.c",

    "include/myos.h",
    "include/os_types.h",
    "include/mpu.h",
    "include/uart.h",

    "app/main.c",
    "app/tasks/task_sensor.c",
    "app/tasks/task.c"
)

foreach ($f in $files) {
    New-Item -ItemType File -Path $f -Force | Out-Null
}

Write-Host "MyOS_Project folder structure created successfully!"

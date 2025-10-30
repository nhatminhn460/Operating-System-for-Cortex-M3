#include "process.h"
#include "uart.h"

#define SCB_SHPR3 (*(volatile uint32_t*)0xE000ED20)

extern void switch_context(uint32_t **old_sp_ptr, uint32_t *new_sp);
extern void start_first_task(uint32_t *first_sp);

static uint32_t stacks[MAX_PROCESSES][STACK_SIZE];
static PCB_t pcb_table[MAX_PROCESSES];
static int current = -1;
static int total_processes = 0;

const char* process_state_str(process_state_t state) {
    switch (state) {
        case PROC_NEW:        return "NEW";
        case PROC_READY:      return "READY";
        case PROC_RUNNING:    return "RUNNING";
        case PROC_WAITING:    return "WAITING";
        case PROC_TERMINATED: return "TERMINATED";
        default:              return "UNKNOWN";
    }
}

void process_init(void) {
    uart_print("Process system initialized.\r\n");
    SCB_SHPR3 |= (0xFF << 16) | (0xFF << 24);
    total_processes = 0;
    current = -1;
}

// assume 'stacks' là uint32_t stacks[MAX_PROCESSES][STACK_SIZE];

void process_create(void (*func)(void), uint32_t pid) {
    if (pid >= MAX_PROCESSES) return;

    pcb_table[pid].pid = pid;
    pcb_table[pid].entry = func;
    pcb_table[pid].state = PROC_NEW;

    // start at top of stack (point _after_ array)
    uint32_t *sp = &stacks[pid][STACK_SIZE]; // points one past last element

    // ---- reserve space for R4-R11 (will be saved/restored by our ASM)
    for (int i = 0; i < 8; ++i) {
        *(--sp) = 0; // placeholders for R11..R4 (content not important)
    }

    *(--sp) = 0x01000000;         // xPSR
    *(--sp) = (uint32_t)func;     // PC (điểm bắt đầu hàm)
    *(--sp) = 0;                  // LR (ignored)
    *(--sp) = 0;                  // R12
    *(--sp) = 0;                  // R3
    *(--sp) = 0;                  // R2
    *(--sp) = 0;                  // R1
    *(--sp) = 0;                  // R0
    for (int i = 0; i < 8; ++i) 
    {
        *(--sp) = 0; // R4-R11 
    }

    // Now 'sp' points to where R4 would be stored (lowest addr of reserved region)
    pcb_table[pid].stack_ptr = sp;
    pcb_table[pid].state = PROC_READY;
    total_processes++;

    uart_print("Created process ");
    uart_print_dec(pid);
    uart_print(" -> state: ");
    uart_print(process_state_str(pcb_table[pid].state));
    uart_print("\r\n");
}

void process_set_state(uint32_t pid, process_state_t new_state) {
    if (pid >= total_processes) return;
    pcb_table[pid].state = new_state;
}

void process_schedule(void) {
    if (total_processes == 0) return;

    int next = (current + 1) % total_processes;
    int found = 0;

    // tìm tiến trình READY
    for (int i = 0; i < total_processes; i++) {
        int idx = (next + i) % total_processes;
        if (pcb_table[idx].state == PROC_READY) {
            found = 1;
            next = idx;
            break;
        }
    }

    if (!found) return;

    if (current >= 0 && pcb_table[current].state == PROC_RUNNING)
        pcb_table[current].state = PROC_READY;

    pcb_table[next].state = PROC_RUNNING;

    uart_print("Switching to process ");
    uart_print_dec(next);
    uart_print(" (");
    uart_print(process_state_str(pcb_table[next].state));
    uart_print(")\r\n");

    // khởi động lần đầu tiên
    if (current == -1) 
    {
        current = next;
        start_first_task(pcb_table[next].stack_ptr);
    } 
    else 
    {
        int old = current;   
        current = next;         
        switch_context(&pcb_table[old].stack_ptr, pcb_table[next].stack_ptr);
    }
}



#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>

#define MAX_PROCESSES 5
#define STACK_SIZE 256

typedef enum {
    PROC_NEW,
    PROC_READY,
    PROC_RUNNING,
    PROC_WAITING,
    PROC_TERMINATED
} process_state_t;

typedef struct {
    uint32_t *stack_ptr;       // con trỏ stack hiện tại
    uint32_t pid;              // ID tiến trình
    process_state_t state;     // trạng thái tiến trình
    void (*entry)(void);       // hàm chính của tiến trình
} PCB_t;

void process_init(void);
void process_create(void (*func)(void), uint32_t pid);
void process_schedule(void);
void process_set_state(uint32_t pid, process_state_t new_state);
const char* process_state_str(process_state_t state);

#endif

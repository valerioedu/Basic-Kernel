#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../../Libs/BasicLibrary/int.h"

typedef enum {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_FINISHED
} thread_status;
  
typedef struct Task {
    void (*task)(void);
    uint64_t priority;
    uint64_t period;
    uint64_t deadline;
    uint64_t start_time;
    uint64_t end_time;
    thread_status status;
    uint64_t stack_pointer;
    uint64_t stack_size;
    uint64_t stack_region;
    struct Task* next;
} Task;                                         //copilot suggested this struct, might be useful

#endif
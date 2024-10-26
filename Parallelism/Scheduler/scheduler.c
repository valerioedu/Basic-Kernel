/*
    * scheduler.c
    *
    * This will be the scheduler for just one thread, since multithreading will be implemented in rust
*/

#include "scheduler.h"
#include "../../Libs/BasicLibrary/basic_library.h"

void scheduler(Process** process_list) {
    Process idle = {
        .pid = 0,
        .size = 0,
        .state = PROCESS_READY,
        .priority = ZEROPRIORITY,
        .pages = NULL,
        .terminated = false,
        .next = NULL,
        .ctx = {0}
    };

    while (true) {
        for (int i = 0; i < process_count; i++) {
            for (int j = 0; j < process_count - i - 1; j++) {
                if (process_list[j]->priority > process_list[j + 1]->priority) {
                    Process* temp = process_list[j];
                    process_list[j] = process_list[j + 1];
                    process_list[j + 1] = temp;
                }
            }
        }

        bool found = false;
        for (int i = 0; i < process_count; i++) {
            if (process_list[i]->state == PROCESS_READY) {
                if (current_process != process_list[i]) {
                    if (current_process != NULL) {
                        current_process->state = PROCESS_READY;
                    }
                    process_list[i]->state = PROCESS_RUNNING;
                    context_switch(process_list[i]);
                }
                found = true;
                break;
            }
        }

        if (!found) {
            if (current_process != &idle) {
                if (current_process != NULL) {
                    current_process->state = PROCESS_READY;
                }
                idle.state = PROCESS_RUNNING;
                context_switch(&idle);
            }
        }
    }
}

void switch_to_thread_c(size_t thread_index) {
    extern Process* thread_list;
    extern size_t thread_count;

    if (thread_index >= thread_count) {
        panic("Thread index out of bounds\n");
    }

    Process* next_thread = thread_list[thread_index];
    if (current_process != NULL) {
        save_context(&current_process->ctx);
        current_process->state = PROCESS_READY;
    }
    current_process = next_thread;
    current_process->state = PROCESS_RUNNING;
    restore_context(&current_process->ctx);
}
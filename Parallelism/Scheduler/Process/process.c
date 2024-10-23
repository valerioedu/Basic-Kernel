/*
    * process.c
    *
    * This file will contain the process structure and the functions to manage the processes
    * 
*/

#include "process.h"
#include "../../../Libs/BasicLibrary/basic_library.h"
#include "../../../FileSystem/MemoryAllocator/memoryallocator.h"

Process** process_list = NULL;
static int process_count = 0;
static int process_capacity = 0;
Process* current_process = NULL;

void dynamic_process_list() {
    if (process_capacity == 0) {
        process_capacity = RAM_SIZE / PAGES_SIZE;
        process_list = (Process**)mmalloc(process_capacity * sizeof(Process*));
    } else {
        int empty_slot = 0;
        for (int i = 0; i < process_capacity; i++) {
            if (process_list[0] != NULL) {
                process_count++;
                continue;
            }
            if (process_list[i] == process_list[i-1] && i > 0) {
                continue;
            } 
            if (process_list[i] != process_list[i-1] && i > 0) {
                process_count++;
            }
            if (process_list[i] == NULL) {
                empty_slot = 1;
                Process** new_process_list = (Process**)mmalloc((process_count +(RAM_SIZE - i * PAGES_SIZE)) / PAGES_SIZE * sizeof(Process*));
                ffree(process_list);
                process_list = new_process_list;
                ffree(new_process_list);
                break;
            }
            if (!empty_slot) {
                panic("Ram is already full\n");
            }
        }
    }
}

Process* create(uint64_t pid, uint64_t size, ProcessPriority priority, void (*entry_point)(void)) {
    Process* process = (Process*)mmalloc(sizeof(Process));
    if (process == NULL) {
        panic("Failed to allocate memory to the process\n");
    }
    process->pid = pid;
    process->size = size;
    process->state = PROCESS_READY;
    process->priority = priority;
    process->pages = NULL;
    process->next = NULL;
    uint64_t pages_count = size / PAGES_SIZE;
    if (size % PAGES_SIZE != 0) {
        pages_count++;
    }
    for (uint64_t i = 0; i < pages_count; i++) {
        Page* page = (Page*)mmalloc(sizeof(Page));
        if (page == NULL) {
            panic("Failed to allocate memory to the page\n");
        }
        page->frame = i;
        page->pid = process->pid;
        page->virtual_address = i * PAGES_SIZE;
        page->next = NULL;
        if (process->pages == NULL) {
            process->pages = page;
        } else {
            Page* temp = process->pages;
            while (temp->next != NULL) {
                temp = temp->next;
            }
            temp->next = page;
        }
    }

    if ( process_list == NULL) {
        process_list = process;
    } else {
        Process* temp = process_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = process;
    }
    process_list[process_count++] = process;
    dynamic_process_list();
    return process;
}

void destroy(Process* process) {
    if (process == NULL) {
        printf("Process is NULL\n");
        return;
    }
    Page* page = process->pages;
    while (page != NULL) {
        Page* temp = page->next;
        ffree((size_t)page);
        page = temp;
    }

    for (int i = 0; i < process_count; i++) {
        if (process_list[i] == process) {
            for (int j = i; j < process_count - 1; j++) {
                process_list[j] = process_list[j + 1];
            }
            process_list[process_count - 1] = NULL;
            process_count--;
            break;
        }
    }

    ffree(sizeof(process));
    dynamic_process_list();
}

void save_context(struct context* ctx) {
    asm volatile(
        "str x19, [%0, #0]\n"
        "str x20, [%0, #8]\n"
        "str x21, [%0, #16]\n"
        "str x22, [%0, #24]\n"
        "str x23, [%0, #32]\n"
        "str x24, [%0, #40]\n"
        "str x25, [%0, #48]\n"
        "str x26, [%0, #56]\n"
        "str x27, [%0, #64]\n"
        "str x28, [%0, #72]\n"
        "str x29, [%0, #80]\n"
        "str x30, [%0, #88]\n"
        "mov %1, sp\n"
        "str %1, [%0, #96]\n"
        : : "r"(ctx), "r"(ctx->sp) : "memory"
    );
}

void restore_context(struct context* ctx) {
    asm volatile(
        "ldr x19, [%0, #0]\n"
        "ldr x20, [%0, #8]\n"
        "ldr x21, [%0, #16]\n"
        "ldr x22, [%0, #24]\n"
        "ldr x23, [%0, #32]\n"
        "ldr x24, [%0, #40]\n"
        "ldr x25, [%0, #48]\n"
        "ldr x26, [%0, #56]\n"
        "ldr x27, [%0, #64]\n"
        "ldr x28, [%0, #72]\n"
        "ldr x29, [%0, #80]\n"
        "ldr x30, [%0, #88]\n"
        "ldr %1, [%0, #96]\n"
        "mov sp, %1\n"
        : : "r"(ctx), "r"(ctx->sp) : "memory"
    );
}

void context_switch(Process* next) {
    if (current_process != NULL) {
        save_context(&current_process->ctx);
        current_process->state = PROCESS_READY;
    }
    current_process = next;
    current_process->state = PROCESS_RUNNING;
    restore_context(&current_process->ctx);
}
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
        //TODO: Implement dynamic process list
        process_capacity *= 2; // just for testing
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
}
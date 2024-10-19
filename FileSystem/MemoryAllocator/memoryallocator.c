#include "memoryallocator.h"
#include "../Libs/BasicLibrary/basic_library.h"

void dynamic_stack_pointer_allocation(Stack** sp_ptr) {
    if (*sp_ptr != NULL) {
        free(*sp_ptr);
    }                                                   //free the old stack pointer
    *sp_ptr = (Stack*)malloc(sizeof(Stack));
    if (*sp_ptr == NULL) {
        panic("Failed to allocate memory to the stack pointer\n");
    }                                                   //check if the memory is allocated
    (*sp_ptr)->sp = (uint64_t*)sp;
    (*sp_ptr)->size = 8;
    (*sp_ptr)->region = RAM_SIZE - (*sp_ptr)->size;
    (*sp_ptr)->start_address = sp - (*sp_ptr)->size;
    (*sp_ptr)->end_address = sp;
    if ((*sp_ptr)->end_address == NULL || (*sp_ptr)->end_address > RAM_SIZE) {
        panic("Failed to allocate memory to the stack pointer\n");
        free(*sp_ptr);
        free(sp_ptr);
        *sp_ptr = NULL;;
    }                                                   //check if the memory is allocated
    (*sp_ptr)->next = NULL;
}                                                       //dinamically allocate memory to the stack pointer when called

void init_stack_pointer() {
    Stack* sp_ptr = NULL;
    dynamic_stack_pointer_allocation(&sp_ptr);
}                                                       //initialize the stack pointer

void check_priority() {

}

void dynamic_memory_allocation(uint64_t size, ProcessPriority priority) {
    if (size > RAM_SIZE) {
        panic("Memory size exceeds the RAM size\n");
    }                                                   //check if the memory size exceeds the RAM size, need to implement virtual memory
    Process* process = (Process*)malloc(sizeof(Process));
    if (process == NULL) {
        panic("Failed to allocate memory to the process\n");
    }                                                   //check if the memory is allocated
    process->pid = process_count++;
    process->size = size;
    process->state = PROCESS_READY;
    process->priority = priority;
    process->pages = NULL;
    process->next = NULL;
    if (process_list == NULL) {
        process_list = process;
    } else {
        Process* temp = process_list;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = process;
    }                                                   //add the process to the process list
    uint64_t pages_count = size / PAGES_SIZE;
    if (size % PAGES_SIZE != 0) {
        pages_count++;
    }                                                   //calculate the number of pages
    for (uint64_t i = 0; i < pages_count; i++) {
        Page* page = (Page*)malloc(sizeof(Page));
        if (page == NULL) {
            panic("Error: Failed to allocate memory to the page\n");
        }                                               //check if the memory is allocated
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
    }                                                   //add the pages to the process
}                                                       //dinamically allocate memory to the process when called

/*
    * memoryallocator.c
    * 
    * This is a simple stack pointer and memory allocator program that i wrote some days ago.
    *
    * I only changed exit(1) to panic()
    * 
    * However, I need to implement a malloc and free functions
*/
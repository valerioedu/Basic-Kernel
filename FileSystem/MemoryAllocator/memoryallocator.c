#include "memoryallocator.h"
#include "../Libs/BasicLibrary/basic_library.h"

#define HEAP_SIZE 64*1024*1024                          //Usually it'd be 1/64 of the size, however I think 64mb is enough

static uint16_t heap[HEAP_SIZE];                        //Heap array
static size_t heap_index = 0;                           //Heap index

static uint8_t stack[2*HEAP_SIZE];                      // Stack array, for simplicity it is 2x size of the heap
static size_t stack_index = HEAP_SIZE;                  // Stack index at the end of the stack

void* mmalloc(size_t size) {
    if (heap_index + size > HEAP_SIZE) {
        panic("Memory size exceeds the heap size\n");
    }                                                   //check if the memory size exceeds the heap size
    void* ptr = &heap[heap_index];
    heap_index += size;
    return ptr;
}

void ffree(size_t size) {
    if (heap_index >= size) {
        heap_index -= size;
    } else {
        panic("Invalid size\n");
    }
}

void stack_free(size_t size) {
    if (stack_index + size <= 2*HEAP_SIZE) {
        stack_index += size;
    } else {
        panic("Invalid size\n");
    }
}

void* stack_alloc(size_t size) {
    if (stack_index < size) {
        panic("Stack size exceeds the allocated stack size\n");
    }
    stack_index -= size;
    return &stack[stack_index];
}

void dynamic_stack_pointer_allocation(Stack** sp_ptr) {
    if (*sp_ptr != NULL) {
        ffree(*sp_ptr);
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
        ffree(*sp_ptr);
        ffree(sp_ptr);
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
*/
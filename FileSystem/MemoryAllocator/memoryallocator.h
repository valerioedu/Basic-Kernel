#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H

#include "../Libs/BasicLibrary/int.h"
#include "../Libs/BasicLibrary/basic_library.h"
#include "../../Parallelism/Scheduler/Process/process.h"

#define RAM_SIZE 8589934592                             //ram size of the raspberry pi 5
#define PAGES_SIZE 4096                                 //size of a page of the block of memory

typedef struct Page {
    uint64_t frame;                                     //frame of the page
    uint64_t pid;                                       //process id
    uint64_t virtual_address;                           //virtual address of the page
    struct Page* next;                                  //next page
} Page;

typedef struct Block {
    uint64_t size;                                      //size of the block
    uint64_t start_address;                             //start address of the block
    uint64_t end_address;                               //end address of the block
    struct Block* next;                                 //next block
} Block;

typedef enum {
    PROCESS_READY,                                      // Process is ready to be executed
    PROCESS_WAITING,                                    // Process is in waiting state
    PROCESS_RUNNING,                                    // Process is currently running
    PROCESS_TERMINATED                                  // Process has terminated
} ProcessState;

typedef enum {
    ZEROPRIORITY,                                       //zero priority
    NORMALPRIORITY,                                     //normal priority
    MAXPRIORITY,                                        //max priority
} ProcessPriority;

//typedef struct Process {
//    uint64_t pid;                                       //process identifier
//    uint64_t size;                                      //memory size of the process
//    ProcessState state;                                 //state of the process
//    ProcessPriority priority;                           //priority of the process
//    Page* pages;                                        //pages of the process(size/PAGES_SIZE)
//    struct Process* next;                               //next process
//} Process;

typedef struct Stack {
    uint64_t* sp;                                       // Stack pointer
    uint64_t size;                                      // Size dedicated to the stack
    uint64_t region;                                    // Region of the stack portion (for sp RAM_SIZE - sp_size)
    uint64_t start_address;                             // Start address of the stack
    uint64_t end_address;                               // End address of the stack
    struct Stack* next;                                 // Next stack pointer
} Stack;

typedef struct BlockMetaData {
    void* ptr;                                          // Pointer to the block
    size_t size;                                        // Size of the block
    struct BlockMetaData* next;                         // Next block
} BlockMetaData;

uint64_t sp = 0x1FFFFFFFF;                              //first stack pointer
size_t process_count = 0;                               //process count
Process** process_list = NULL;                          //process list

void* mmalloc(size_t size);
void ffree(size_t size);

#endif
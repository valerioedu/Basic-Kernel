#ifndef PROCESS_H
#define PROCESS_H

#include "../../../Libs/BasicLibrary/int.h"
#include "../../../Libs/BasicLibrary/basic_library.h"

#define RAM_SIZE 8*1024*1024*1024
#define PAGES_SIZE 4096

typedef struct Page {
    uint64_t frame;
    uint64_t pid;
    uint64_t virtual_address;
    struct Page* next;
} Page;

typedef enum {
    PROCESS_READY,
    PROCESS_WAITING,
    PROCESS_RUNNING,
    PROCESS_TERMINATED
} ProcessState;

typedef enum {
    ZEROPRIORITY,
    NORMALPRIORITY,
    MAXPRIORITY
} ProcessPriority;

typedef struct Process {
    uint64_t pid;
    uint64_t size;
    ProcessState state;
    ProcessPriority priority;
    Page* pages;
    struct Process* next;
} Process;

int process_count;

Process* create(uint64_t pid, uint64_t size, ProcessPriority priority, void (*entry_point)(void));
void destroy(Process* process);

#endif
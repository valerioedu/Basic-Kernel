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

struct context {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
    uint64_t sp;
};

typedef enum {
    ZEROPRIORITY,
    NORMALPRIORITY,
    MAXPRIORITY
} ProcessPriority;

typedef struct Process {
    uint64_t pid;
    uint64_t size;
    Process* parent;
    ProcessState state;
    ProcessPriority priority;
    Page* pages;
    bool terminated;
    struct context ctx;
    struct Process* next;
} Process;

Process** process_list = NULL;
static int process_count = 0;
static int process_capacity = 0;
Process* current_process = NULL;


Process* create(uint64_t pid, uint64_t size, ProcessPriority priority, void (*entry_point)(void));
void destroy(Process* process);

#endif
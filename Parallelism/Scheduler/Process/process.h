#ifndef PROCESS_H
#define PROCESS_H

#include "../../../Libs/BasicLibrary/int.h"
#include "../../../Libs/BasicLibrary/basic_library.h"

#define RAM_SIZE 8*1024*1024*1024
#define PAGES_SIZE 4096
#define MAX_PROCESSES 4*32                          //32 processes per core (overkill for an os like this but anyway)


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
Process* process_list[MAX_PROCESSES];             
Process* current_process = NULL;


Process* create(uint64_t pid, uint64_t size, ProcessPriority priority, void (*entry_point)(void));
void destroy(Process* process);
void dynamic_process_list();
void save_context(struct context* ctx);
void restore_context(struct context* ctx);
void context_switch(Process* next);
Process* find_process(uint64_t pid);

#endif
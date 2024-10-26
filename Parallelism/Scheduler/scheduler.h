#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../../Libs/BasicLibrary/int.h"
#include "../../Libs/BasicLibrary/basic_library.h"
#include "Process/process.h"

#define MAX_PROCESSES 4*32                          //32 processes per core (overkill for an os like this but anyway)
Process* process_list[MAX_PROCESSES];             
Process* current_process = NULL;

void scheduler(Process** process_list);

#endif
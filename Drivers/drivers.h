#ifndef DRIVERS_H
#define DRIVERS_H

#include "../Libs/BasicLibrary/int.h"
#include "../Libs/BasicLibrary/basic_library.h"
#include "../Libs/BasicLibrary/uart.h"

typedef struct Device {
    char* name;
    void* port;
    struct Device* next;
} Device;

#endif
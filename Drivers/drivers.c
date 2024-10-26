#include "drivers.h"

#define USB_BASE 0x7E980000

Device* devices = NULL;

void add_device(char* name, void* port) {
    Device* device = (Device*)mmalloc(sizeof(Device));
    if (device == NULL) {
        panic("Failed to allocate memory to the device\n");
    }
    device->name = name;
    device->port = port;
    device->next = NULL;
    if (devices == NULL) {
        devices = device;
    } else {
        Device* temp = devices;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = device;
    }
}

init_keyboard() {
    add_device("keyboard", 0x7E201000);
}
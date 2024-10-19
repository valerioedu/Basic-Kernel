#include "uart.h"
#include "basic_library.h"

void write(char *str) {
    int i = 0;
    for (i = 0; str[i] != '\0'; i++) {
        uart_putc(str[i]);
    }
}

void read(char *buffer, int size) {
    int i = 0;
    char c;
    while (i < size - 1) {
        uart_getc(&c);
        if (c == '\n' || c == '\r') {
            buffer[i] = '\0';
            break;
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}
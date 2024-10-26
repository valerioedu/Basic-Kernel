#include "uart.h"

void uart_init(void) {
    *(volatile unsigned int *)UARTCR = 0x00000000;
    *(volatile unsigned int *)UARTIBRD = 1;
    *(volatile unsigned int *)UARTFBRD = 40;
    *(volatile unsigned int *)UARTLCR_H = (1 << 4) | (1 << 5) | (1 << 6);
    *(volatile unsigned int *)UARTCR = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
    while (*(volatile unsigned int *)UARTFR & (1 << 5)) {}
    *(volatile unsigned int *)UARTDR = c;
}

void uart_getc(char *c) {
    while (*(volatile unsigned int *)UARTFR & (1 << 4)) {}
    *c = *(volatile unsigned int *)UARTDR;
}
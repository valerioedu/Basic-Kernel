#include "uart.h"

#define UART0_BASE 0x3F201000
#define UARTDR     (UART0_BASE + 0x00)
#define UARTFR     (UART0_BASE + 0x18)
#define UARTIBRD   (UART0_BASE + 0x24)
#define UARTFBRD   (UART0_BASE + 0x28)
#define UARTLCR_H  (UART0_BASE + 0x2C)
#define UARTCR     (UART0_BASE + 0x30)

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
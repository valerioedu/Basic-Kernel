#ifndef UART_H
#define UART_H

#define UART0_BASE 0x7E201000
#define UARTDR     (UART0_BASE + 0x00)
#define UARTFR     (UART0_BASE + 0x18)
#define UARTIBRD   (UART0_BASE + 0x24)
#define UARTFBRD   (UART0_BASE + 0x28)
#define UARTLCR_H  (UART0_BASE + 0x2C)
#define UARTCR     (UART0_BASE + 0x30)


void uart_init(void);
void uart_putc(char c);
void uart_getc(char *c);

#endif
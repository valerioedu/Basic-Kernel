#include "uart.h"
#include "basic_library.h"
#include "stdarg.h"

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

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == 'd') {
                int num = va_arg(args, int);
                char num_str[11];
                int i = 0;
                if (num == 0) {
                    num_str[i++] = '0';
                } else {
                    if (num < 0) {
                        num_str[i++] = '-';
                        num = -num;
                    }
                    int j;
                    for (j = 1000000000; j > num; j /= 10);
                    for (; j > 0; j /= 10) {
                        num_str[i++] = num / j + '0';
                        num %= j;
                    }
                }
                num_str[i] = '\0';
                write(num_str);
            } else if (*format == 's') {
                char *str = va_arg(args, char *);
                write(str);
            }
        } else {
            uart_putc(*format);
        }
        format++;
    }

    va_end(args);
}

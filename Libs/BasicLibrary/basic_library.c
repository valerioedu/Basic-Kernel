#include "uart.h"
#include "basic_library.h"
#include "stdarg.h"
#include "../FileSystem/MemoryAllocator/memoryallocator.h"

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

void scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == 'd') {
                int *num = va_arg(args, int *);
                char num_str[11];
                read(num_str, 11);
                int i = 0;
                int sign = 1;
                if (num_str[i] == '-') {
                    sign = -1;
                    i++;
                }
                *num = 0;
                while (num_str[i] != '\0') {
                    *num = *num * 10 + num_str[i] - '0';
                    i++;
                }
                *num *= sign;
            } else if (*format == 's') {
                char *str = va_arg(args, char *);
                read(str, 100);
            }
        }
        format++;
    }

    va_end(args);
}

void panic(char *message) {
    write("Kernel panic: ");
    write(message);
    write("\n");
    while (1) {
        asm("wfi");
    }
}

FILE *ffopen(const char *filename, char *mode) {
    FILE *file = (FILE *)mmalloc(sizeof(FILE));
    if (file == NULL) {
        panic("Failed to allocate memory to the file");
    }
    file->name = strdup(filename);
    if (file->name == NULL) {
        ffree(file);
        panic("Failed to allocate memory to the file name");
    }   
    file->mode = strdup(mode);

    return file;
}

bool strcompare(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return false;
        }
        str1++;
        str2++;
    }
    return *str1 == '\0' && *str2 == '\0';
}

void exit(int status) {
    while (1) {
        asm("wfi");
    }
}

strcpy(char *dest, const char *src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

strdup(const char *str) {
    char *new_str = (char *)mmalloc(strlen(str) + 1);
    if (new_str == NULL) {
        panic("Failed to allocate memory to the string");
    }
    strcpy(new_str, str);
    return new_str;
}
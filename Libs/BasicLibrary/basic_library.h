#ifndef BASIC_LIBRARY_H
#define BASIC_LIBRARY_H

void write(char *str);
void read(char *buffer, int size);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-library-redeclaration"

void printf(const char *format, ...);

#pragma GCC diagnostic pop

#endif
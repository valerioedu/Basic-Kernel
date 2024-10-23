#ifndef BASIC_LIBRARY_H
#define BASIC_LIBRARY_H

#include "../FileSystem/FS/fs.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef enum {
    false,
    true
} bool;

typedef struct FILE {
    char *name;
    char *mode;
    char *buffer;
    int size;
    Node* parent;
    int eof;
    struct FILE *next;
} FILE;

void write(char *str);
void read(char *buffer, int size);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-library-redeclaration"

void printf(const char *format, ...);
void scanf(const char *format, ...);
FILE* ffopen(const char *name, const char *mode);

#pragma GCC diagnostic pop

#endif
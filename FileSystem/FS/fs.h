#ifndef FS_H
#define FS_H

#include "../../Libs/BasicLibrary/int.h"
#include "../../Libs/BasicLibrary/basic_library.h"

typedef struct Node {
    char* name;
    struct Node* parent;
    struct Node** children;
    struct Node* next;
} Node;

Node* root;

void init_fs();
Node* mkdir(char* name);
FILE* mkfile(char* name);
void ls();
void cd(char* name);

#endif
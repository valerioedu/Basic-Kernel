#include "fs.h"
#include "../../Libs/BasicLibrary/basic_library.h"
#include "../MemoryAllocator/memoryallocator.h"
#include "../../Libs/BasicLibrary/int.h"

Node* mkdir(char* name) {
    Node* node = (Node*)mmalloc(sizeof(Node));
    if (node == NULL) {
        panic("Failed to allocate memory to the node\n");
    }
    node->name = name;
    node->parent = NULL;
    node->children = NULL;
    node->next = NULL;
    if (root->children == NULL) {
        root->children = node;
    } else {
        Node* temp = root->children;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
    }
    return node;
}                                                                       //temporary mkdir function made by copilot

Node* init_users();

void init_fs() {
    root = (Node*)mmalloc(sizeof(Node));
    root->name = "/";
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;
    init_log;
    init_users;
}

FILE* mkfile(char* name) {
    FILE* file = (FILE*)mmalloc(sizeof(FILE));
    if (file == NULL) {
        panic("Failed to allocate memory to the file");
    }
    file->name = name;
    file->mode = "a";
    file->parent = NULL;
    file->next = NULL;
    //...
    return file;
}

Node* init_users() { 
    Node* Users = (Node*)mmalloc(sizeof(Node));
    if (Users == NULL) {
        panic("Failed to allocate memory to the Users directory\n");
    }
    Users->name = "Users";
    Users->parent = root;
    Users->children = NULL;
    Users->next = NULL;
    root->children[1] = Users;
    FILE *txtfile = (FILE*)mmalloc(sizeof(FILE));
    txtfile->parent = Users;
    txtfile->name = "Users.txt";
    //...
}

Node* init_log() {
    Node* log = (Node*)mmalloc(sizeof(Node));
    if (log == NULL) {
        panic("Failed to allocate memory to the Users directory\n");
    }
    log->name = "Users";
    log->parent = root;
    log->children = NULL;
    log->next = NULL;
    root->children[0] = log;
    FILE *txtfile = (FILE*)mmalloc(sizeof(FILE));
    txtfile->parent = log;
    txtfile->name = "log.txt";
    //...
}

#ifndef SHELL_H
#define SHELL_H

#include "../Libs/BasicLibrary/int.h"
#include "../Libs/BasicLibrary/basic_library.h"

void shell_prompt();
void exec_cmd(char* cmd);
void shell_loop();

#endif
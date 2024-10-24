#include "../Libs/BasicLibrary/basic_library.h"
#include "shell.h"
#include "../Parallelism/Scheduler/Process/process.h"

Process shell = {
    .pid = 0,
    .size = 0,
    .state = PROCESS_RUNNING,
    .priority = MAXPRIORITY,
    .pages = NULL,
    .terminated = false,
    .next = NULL
};

void shell_prompt() {
    printf("shell> ");
}

void exec_cmd(char* cmd) {
    if (strcompare(cmd, "help")) {
        printf("Available commands:\n");
        printf("help - display this help message\n");
        printf("list - list all the processes\n");
        printf("switch - switch to a process\n");
        printf("exit - exit the shell\n");
    } else if (strcompare(cmd, "exit")) {
        printf("Goodbye!\n");
        exit(0);
    } else if ( strcompare(cmd, "list")) {
        printf("List of processes:\n");
        for (int i = 0; i < process_count; i++) {
            printf("Process %d\n", process_list[i]->pid);
        }
    } else if (strcompare(cmd, "switch")) {
        printf("Enter the process id: ");
        int pid;
        scanf("%d", &pid);
        for (int i = 0; i < process_count; i++) {
            if (process_list[i]->pid == pid) {
                current_process = process_list[i];
                printf("Switched to process %d\n", pid);
                return;
            }
        }
        printf("Process not found\n");
    } else {
        printf("Command not found\n");
    }
}

void shell_loop() {
    while (1) {
        shell_prompt();
        char cmd[256];
        read(cmd, 256);
        exec_cmd(cmd);
    }
}
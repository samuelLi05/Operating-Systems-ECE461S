/* child_process.h
    Functions for creating and handling child processes
 */

#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include <stdbool.h>
#include <unistd.h>

typedef struct process {
    char** argv;
    char* out_file;
    char* in_file;
    char* err_file;
} process;

int execOneChild(process* proc); // returns cpid of the child process

void execTwoChildren(process* proc1, process* proc2, int* cpid1, int* cpid2); // creates two children processes connected by a pipe

void get_fd(process* proc, int* open_fd, int* in_fd, int* err_fd); // returns the file descriptor for redirection, -1 if none

int waitForChild(int cpid, int background); // waits for the child process to finish if foreground

#endif /* CHILD_PROCESS_H */
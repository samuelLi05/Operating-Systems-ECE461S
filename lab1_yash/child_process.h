/* child_process.h
    Functions for creating and handling child processes
 */

#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include <stdbool.h>

typedef struct process {
    char** argv;
    char* out_file;
    char* in_file;
    char* err_file;
    bool pipe_arg_1;
    bool pipe_arg_2;
} process;

int execOneChild(process* proc); // returns cpid of the child process

#endif /* CHILD_PROCESS_H */
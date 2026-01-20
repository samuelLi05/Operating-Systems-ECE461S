#include <stdlib.h>
#include "child_process.h"

int execOneChild(process* proc) {
    int cpid = fork();
    if (cpid == 0) {
        // In child process
        int ret = execvp(proc->argv[0], proc->argv);
        // If execvp returns, there was an error
        if (ret == -1) exit(-1);
    }
    return -1;
}
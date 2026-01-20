#include <stdlib.h>
#include "child_process.h"

int execOneChild(process* proc, int pipe_forwarding[]) {
    int cpid = fork();
    if (cpid == 0) {
        // In child process
        // handle pipes anf forwarding here using pipe_forwarding array if needed

        // handle redirections here using proc->in_file, proc->out_file, proc->err_file
        
        int ret = execvp(proc->argv[0], proc->argv);
        // If execvp returns, there was an error
        if (ret == -1) exit(-1);
    }
    return -1;
}
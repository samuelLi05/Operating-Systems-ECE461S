#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "signal_handler.h"

void sig_handler(int signo) {
    if (signo == SIGCHLD)
    {
        // reap terminated child processes
        printf("TODO: Reap child processes\n");
    }
}
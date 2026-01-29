#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "signal_handler.h"
#include "jobs.h"

void sig_handler(int signo) {
    int status;
    int pid;
    // reap the child processes if they have been terminated
    if (signo == SIGCHLD) { // other signals are set as default execution behavior
        while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
            for (int i = 0; i < 20; i++) {
                if (jobs_list[i] != NULL && jobs_list[i]->pgid == pid) { // upon process creation process group numbers are set to the child's pid
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        remove_job(jobs_list[i]->job_id); // remove the job if exited or terminated from receipt signal not caught
                    } else if (WIFSTOPPED(status))
                    {
                        jobs_list[i]->status = STOPPED; // 2 for stopped
                    } else if (WIFCONTINUED(status)) 
                    {
                        jobs_list[i]->status = RUNNING; // 1 for running
                    }
                }
            }
        }
    }
}
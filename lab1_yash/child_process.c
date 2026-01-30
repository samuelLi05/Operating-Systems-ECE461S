#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include "child_process.h"
#include "signal_handler.h"

int execOneChild(process* proc) {
    int out_fd;
    int in_fd;
    int err_fd;
    int pgid;

    // open files for redirection if specified
    get_fd(proc, &out_fd, &in_fd, &err_fd);
    int cpid = fork();

    if (cpid == 0) {
        // In child process
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, sig_handler);
        // handle redirections here using proc->in_file, proc->out_file, proc->err_file
        setpgid(0, 0); // set the child process group ID to its own PID
        if (proc->in_file != NULL) dup2(in_fd, STDIN_FILENO);
        if (proc->out_file != NULL) dup2(out_fd, STDOUT_FILENO);
        if (proc->err_file != NULL) dup2(err_fd, STDERR_FILENO);

        int ret = execvp(proc->argv[0], proc->argv);
        // If execvp returns, there was an error
        if (ret == -1) exit(-1);
    }
    // in parent process
    setpgid(cpid, cpid);
    if (proc->in_file != NULL) close(in_fd);
    if (proc->out_file != NULL) close(out_fd);
    if (proc->err_file != NULL) close(err_fd);
    
    // return child pid
    return cpid;
}

// creates two children processes connected by a pipe
void execTwoChildren(process* proc1, process* proc2, int* cpid1, int* cpid2) {
    // Create a pipe
    int pipe_fd[2];
    pipe(pipe_fd);
    int open_fd1, in_fd1, err_fd1;
    int open_fd2, in_fd2, err_fd2;
    int pgid;

    get_fd(proc1, &open_fd1, &in_fd1, &err_fd1);
    get_fd(proc2, &open_fd2, &in_fd2, &err_fd2);

    *cpid1 = fork();
    if (*cpid1 == 0) {
        // In first child process
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, sig_handler);
        setpgid(0, 0); // set the child process group ID to its own PID
        if (proc1->out_file != NULL) dup2(open_fd1, STDOUT_FILENO);
        if (proc1->in_file != NULL) dup2(in_fd1, STDIN_FILENO);
        if (proc1->err_file != NULL) dup2(err_fd1, STDERR_FILENO);
        // Redirect stdout to pipe write end
        close(pipe_fd[0]); // Close unused read end
        dup2(pipe_fd[1], STDOUT_FILENO);
        execvp(proc1->argv[0], proc1->argv);
        exit(-1);
    }
    setpgid(*cpid1, *cpid1);

    *cpid2 = fork();
    if (*cpid2 == 0) {
        // In second child process
        setpgid(0, *cpid1); // set the second child process group ID to the first child's PID
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGCHLD, sig_handler);
        if (proc2->out_file != NULL) dup2(open_fd2, STDOUT_FILENO);
        if (proc2->in_file != NULL) dup2(in_fd2, STDIN_FILENO);
        if (proc2->err_file != NULL) dup2(err_fd2, STDERR_FILENO);
        // Redirect stdin to pipe read end
        close(pipe_fd[1]); // Close unused write end
        dup2(pipe_fd[0], STDIN_FILENO);

        execvp(proc2->argv[0], proc2->argv);
        exit(-1);
    }
    setpgid(*cpid2, *cpid1);

    close(pipe_fd[0]); // Close read end in parent
    close(pipe_fd[1]); // Close write end in parent
}

// returns the file descriptor for redirection, -1 if none
void get_fd(process* proc, int* open_fd, int* in_fd, int* err_fd) {
    *open_fd = -1;
    *in_fd = -1;
    *err_fd = -1;

    if (proc->out_file != NULL) {
        *open_fd = open(proc->out_file, O_WRONLY | O_CREAT | O_TRUNC , 0644);
    }

    if (proc->in_file != NULL) {
        *in_fd = open(proc->in_file, O_RDONLY);
    }

    if (proc->err_file != NULL) {
        *err_fd = open(proc->err_file, O_WRONLY | O_CREAT | O_TRUNC , 0644);
    }
}

int waitForChild(int cpid, int background) {
    int status;
    if (background == 0) {
        // Wait for the child process to finish
        waitpid(-cpid, &status, WUNTRACED);
        return status;
    } else
    {
        waitpid(-cpid, &status, WNOHANG | WUNTRACED);
        return status;
    }
}
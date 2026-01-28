#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "readline/readline.h"
#include "parse_input.h"
#include "child_process.h"
#include "signal_handler.h"
#include "jobs.h"

int main(void)
{
    int cpid;
    char* read_string;
    char** parsed_input;
    char* command;
    int parsed_input_length;
    int background;
    int foreground;

    // ignore signals for shell to onpy terminate at ctrl d
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGCHLD, sig_handler);

    pid_t shell_pgid = getpid();
    // put shell in its own process group
    setpgid(shell_pgid, shell_pgid);

    // grab control of the terminal
    tcsetpgrp(STDIN_FILENO, shell_pgid);

    while (1) {
    	// 0. Register signal handlers

    	// 1. Print the prompt (#)
        //printf("# ");

    	// 2. Grab and parse the input - NOTE: Make sure to remove the newline
    	// character from the input string (otherwise, you'll pass "ls\n" to
    	// execvp and there is no executable called "ln\n" just "ls")
        read_string = readline("# ");
        if (read_string == NULL) {
            printf("\n");
            break;
        }
        parsed_input = string_parser(read_string, &parsed_input_length);
        if (parsed_input == NULL) {
            free(read_string);
            continue;
        }

    	// 3. Check for job control tokens (fg, bg, jobs, &) (for now just
    	// ignore those commands)
        command = parsed_input[0];
        if (command == NULL) {
            free(read_string);
            free(parsed_input);
            printf("\n"); // print newline for invalid input
            continue; // empty input, prompt again
        }
        // printf("Command entered: %s\n", command); // test simple parsing

        // handle job related commands
        if (strcmp(command, "jobs") == 0) {
            output_jobs_list();
            free(read_string);
            free(parsed_input);
            continue;
        } else if (strcmp(command, "fg") == 0) {
            get_current_job();
            free(read_string);
            free(parsed_input);
            continue;
        } else if (strcmp(command, "bg") == 0) {
            get_current_job();
            free(read_string);
            free(parsed_input);
            continue;
        }

        if (find_background_token(parsed_input) != -1) {
            background = 1;
            foreground = 0;
        } else {
            background = 0;
            foreground = 1;
        }

        
    	// 4. Determine the number of children processes to create (number of
    	// times to call fork) (call fork once per child) (right now this will
    	// just be one)

        // 5. Execute the commands using execvp or execlp - e.g. execOneChild()
    	// or execTwoChildren()

        // find the pipe index if it exists
        int pipe_index = find_pipe_index(parsed_input);
        if (pipe_index == -1) {
            // single process
            process* proc = construct_process(parsed_input, 0, parsed_input_length);
            cpid = execOneChild(proc);
            add_job(cpid, RUNNING, foreground, read_string);
            if (background == 0 && foreground == 1) {
                tcsetpgrp(STDIN_FILENO, cpid);
                waitForChild(cpid, background);
                tcsetpgrp(STDIN_FILENO, shell_pgid);
            } else {
                waitForChild(cpid, background);
            }
            free(proc->argv);
            free(proc);
        } else{
            // two processes connected by a pipe
            process* proc1 = construct_process(parsed_input, 0, pipe_index);
            process* proc2 = construct_process(parsed_input, pipe_index + 1, parsed_input_length);
            int cpid1, cpid2;
            execTwoChildren(proc1, proc2, &cpid1, &cpid2);
            add_job(cpid1, RUNNING, foreground, read_string);
            if (background == 0 && foreground == 1) {
                tcsetpgrp(STDIN_FILENO, cpid1);
                waitForChild(cpid1, 0); // always foreground for piped commands
                tcsetpgrp(STDIN_FILENO, shell_pgid);
            } else {
                waitForChild(cpid1, 0); // always foreground for piped commands
            }
            free(proc1->argv);
            free(proc1);
            free(proc2->argv);
            free(proc2);
        }

    	// 6. NOTE: There are other steps for job related stuff but good luck
    	// we won't spell it out for you
        // printf("\n");
        free(read_string);
        free(parsed_input);
	}

	return 0;
}


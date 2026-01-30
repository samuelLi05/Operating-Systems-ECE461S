#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
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
    char* saved_string;
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
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGCHLD, sig_handler);

    	// 1. Print the prompt (#)
        //printf("# ");

    	// 2. Grab and parse the input - NOTE: Make sure to remove the newline
    	// character from the input string (otherwise, you'll pass "ls\n" to
    	// execvp and there is no executable called "ln\n" just "ls")
        read_string = readline("# ");
        if (read_string == NULL) {
            // free the jobs list before exiting
            free_jobs_list();
            free(read_string);
            printf("\n");
            break;
        }
        saved_string = strdup(read_string); // save a copy for job name
        parsed_input = string_parser(read_string, &parsed_input_length);
        if (parsed_input == NULL) {
            free(read_string);
            free(saved_string);
            free(parsed_input);
            continue;
        }
        

    	// 3. Check for job control tokens (fg, bg, jobs, &) (for now just
    	// ignore those commands)

        // filter the background command if it is present
        if (find_background_token(parsed_input) != -1) {
            background = 1;
            foreground = 0;
             // remove the & token from parsed input
            parsed_input[parsed_input_length - 1] = NULL;
            parsed_input_length--;
        } else {
            background = 0;
            foreground = 1;
        }


        command = parsed_input[0];
        if (command == NULL) {
            free(read_string);
            free(saved_string);
            free(parsed_input);
            printf("\n"); // print newline for invalid input
            continue; // empty input, prompt again
        }
        // printf("Command entered: %s\n", command); // test simple parsing

        // handle job related commands
        if (strcmp(command, "jobs") == 0) {
            output_jobs_list();
            free(read_string);
            free(saved_string);
            free(parsed_input);
            continue;
        } else if (strcmp(command, "fg") == 0) {
            get_current_job();
            if (current_job == NULL) {
                free(read_string);
                free(saved_string);
                free(parsed_input);
                continue;
            }
            // output the command to console as if it was run normally in foreground, remove the & symbol if it was present for printing
            remove_bg_token_from_job_name(current_job);
            printf("%s\n", current_job->job_name);
            // Move job to foreground: give it terminal, continue it, then wait.
            set_job_foreground(current_job->pgid);
            tcsetpgrp(STDIN_FILENO, current_job->pgid);
            kill(-current_job->pgid, SIGCONT);

            int status = waitForChild(current_job->pgid, 0);
            tcsetpgrp(STDIN_FILENO, shell_pgid);

            job* finished_job = find_job_by_pgid(current_job->pgid);
            if (finished_job != NULL)
            {
                if (WIFEXITED(status) || WIFSIGNALED(status))
                {
                    remove_job(finished_job->job_id);
                } else if (WIFSTOPPED(status))
                {
                    finished_job->status = STOPPED;
                    set_job_background(finished_job->pgid);
                    current_stopped_job = finished_job;
                }
            }
            free(read_string);
            free(saved_string);
            free(parsed_input);
            continue;
        } else if (strcmp(command, "bg") == 0) {
            get_recent_stopped_job();
            if (current_stopped_job == NULL) {
                free(read_string);
                free(saved_string);
                free(parsed_input);
                continue;
            }
            // Continue the most recent stopped job in the background.
            current_stopped_job->status = RUNNING;
            set_job_background(current_stopped_job->pgid);
            kill(-current_stopped_job->pgid, SIGCONT);
            add_bg_token_to_job_name(current_stopped_job);
            printf("[%d]%c  %s\t%s\n", current_stopped_job->job_id, '+', "Running", current_stopped_job->job_name);
            free(read_string);
            free(saved_string);
            free(parsed_input);
            continue;
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
            add_job(cpid, RUNNING, foreground, saved_string);
            if (background == 0 && foreground == 1)
            {
                tcsetpgrp(STDIN_FILENO, cpid);
                int status = waitForChild(cpid, background);
                tcsetpgrp(STDIN_FILENO, shell_pgid);
                job* finished_job = find_job_by_pgid(cpid);
                if (finished_job != NULL)
                {
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        remove_job(finished_job->job_id);
                    } else if (WIFSTOPPED(status))
                    {
                        finished_job->status = STOPPED;
                        set_job_background(cpid);
                        current_stopped_job = finished_job;
                    }
                }
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
            add_job(cpid1, RUNNING, foreground, saved_string);
            if (background == 0 && foreground == 1)
            {
                tcsetpgrp(STDIN_FILENO, cpid1);
                int status = waitForChild(cpid1, background); // always foreground for piped commands
                tcsetpgrp(STDIN_FILENO, shell_pgid);

                job* finished_job = find_job_by_pgid(cpid1);
                if (finished_job != NULL)
                {
                    if (WIFEXITED(status) || WIFSIGNALED(status))
                    {
                        remove_job(finished_job->job_id);
                    } else if (WIFSTOPPED(status))
                    {
                        finished_job->status = STOPPED;
                        set_job_background(cpid1);
                        current_stopped_job = finished_job;
                    }
                }
            } else {
                waitForChild(cpid1, background); // always foreground for piped commands
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
        free(saved_string);
        free(parsed_input);
	}

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "readline/readline.h"
#include "parse_input.h"
#include "child_process.h"
int main(void)
{
    int cpid;
    char* read_string;
    char** parsed_input;
    char* command;
    while (1) {
    	// 0. Register signal handlers

    	// 1. Print the prompt (#)

    	// 2. Grab and parse the input - NOTE: Make sure to remove the newline
    	// character from the input string (otherwise, you'll pass "ls\n" to
    	// execvp and there is no executable called "ln\n" just "ls")
        read_string = readline("# "); 
        parsed_input = string_parser(read_string);

    	// 3. Check for job control tokens (fg, bg, jobs, &) (for now just
    	// ignore those commands)

    	// 4. Determine the number of children processes to create (number of
    	// times to call fork) (call fork once per child) (right now this will
    	// just be one)
        command = parsed_input[0];
        if (command == NULL) {
            free(read_string);
            free(parsed_input);
            printf("\n"); // print newline for invalid input
            continue; // empty input, prompt again
        }

        cpid = fork();
        if (cpid == 0){
            // Handle the child process
        }

        

    	// 5. Execute the commands using execvp or execlp - e.g. execOneChild()
    	// or execTwoChildren()

    	// 6. NOTE: There are other steps for job related stuff but good luck
    	// we won't spell it out for you
	}

	return 0;
}


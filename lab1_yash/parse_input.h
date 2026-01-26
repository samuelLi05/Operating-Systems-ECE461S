/* parse_input.h
    Functions for string parsing
 */
#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#include <stddef.h>
#include "child_process.h"

char **string_parser(char* input_string, int* parsed_length);

int find_pipe_index(char** parsed_input);

int find_redir_index(char **parsed_input, char* type, int start_index, int end_index);

// int* find_all_redir_indices(char **parsed_input, int* types, int* count);

int find_background_token(char **parsed_input);

process* construct_process(char** parsed_input, int start_index, int end_index);

#endif /* PARSE_INPUT_H */

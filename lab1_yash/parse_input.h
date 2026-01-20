/* parse_input.h
    Functions for string parsing
 */
#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#include <stddef.h>

char **string_parser(char* input_string);

int find_pipe_index(char** parsed_input);

int find_redir_index(char **parsed_input, int* type);

int* find_all_redir_indices(char **parsed_input, int* types, int* count);

int find_background_token(char **parsed_input);

#endif /* PARSE_INPUT_H */

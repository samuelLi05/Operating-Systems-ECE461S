#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "parse_input.h"
// Parses the input string into an array of strings (tokens) split by spaces
// Returns a NULL-terminated array of strings
char** string_parser(char* input_string)
{
    if (input_string == NULL) {
        return NULL;
    }
    
    // Allocate memory for array of string pointers, realloc as needed
    int array_size = 10; // initial size
    char** string_array = malloc(array_size * sizeof(char*));
    if (string_array == NULL) {
        return NULL;
    }

    int index = 0;
    char* save_ptr = NULL;
    char* i = strtok_r(input_string, " \t\n\r\v\f", &save_ptr);

    while (i)
    {
        // Reallocate if needed
        if (index >= array_size){
            array_size *= 2;
            char** temp_array = realloc(string_array, array_size * sizeof(char*));
            if (temp_array == NULL) {
                free(string_array);
                return NULL;
            }
            string_array = temp_array;
        }

        // add to the output
        string_array[index++] = i;
        i = strtok_r(NULL, " \t\n\r\v\f", &save_ptr);
    }
    string_array[index] = NULL; // null terminate the array
    return string_array;
}

// Finds the index of the pipe character '|' in the parsed input array
int find_pipe_index(char **parsed_input) {
    if (parsed_input == NULL) {
        return -1;
    }
    for (int i = 0; parsed_input[i] != NULL; i++) {
        if (strcmp(parsed_input[i], "|") == 0) {
            return i;
        }
    }
    return -1; // not found
}

// Finds the index of redirection characters in the parsed input array. type should be a redirection character like "<", ">", or ">>"
int find_redir_index(char **parsed_input, char* type) {
    if (parsed_input == NULL) {
        return -1;
    }
    if (strcmp(type, "<") != 0 && strcmp(type, ">") != 0 && strcmp(type, ">>") != 0) {
        return -1; // invalid type
    }
    for (int i = 0; parsed_input[i] != NULL; i++) {
        if (strcmp(parsed_input[i], type) == 0) {
           return i;
        }
    }
    return -1; // not found
}

// Finds all indices of redirection characters in the parsed input array
// int* find_all_redir_indices(char **parsed_input, int* types, int* count) {
//     if (parsed_input == NULL || types == NULL || count == NULL) {
//         return NULL;
//     }

//     int* indices = malloc(10 * sizeof(int)); // initial size
//     if (indices == NULL) {
//         return NULL;
//     }
//     int array_size = 10;
//     *count = 0;

//     for (int i = 0; parsed_input[i] != NULL; i++) {
//         if (strcmp(parsed_input[i], "<") == 0) {
//             types[*count] = 0; 
//         } else if (strcmp(parsed_input[i], ">") == 0) {
//             types[*count] = 1; 
//         } else if (strcmp(parsed_input[i], ">>") == 0) {
//             types[*count] = 2; 
//         }

//         // Reallocate if needed if lots of file redirections
//         if (*count >= array_size) {
//             array_size *= 2;
//             int* temp_indices = realloc(indices, array_size * sizeof(int));
//             if (temp_indices == NULL) {
//                 free(indices); // realloc fail case
//                 return NULL;
//             }
//             indices = temp_indices;
//         }

//         indices[(*count)++] = i;
//     }

//     return indices;
// }

// Finds the index of the background token '&' in the parsed input array
int find_background_token(char **parsed_input) {
    if (parsed_input == NULL) {
        return -1;
    }
    for (int i = 0; parsed_input[i] != NULL; i++) {
        if (strcmp(parsed_input[i], "&") == 0) {
            return i;
        }
    }
    return -1; // not found
}

// Constructs a process struct from the parsed input array between start_index and end_index (exclusive)
process* construct_process(char** parsed_input, int start_index, int end_index) {
    if (parsed_input == NULL || start_index < 0 || end_index <= start_index) {
        return NULL;
    }

    char* in_file = NULL;
    char* out_file = NULL;
    char* err_file = NULL;

    // allocate process struct

    process* proc = malloc(sizeof(process));
    if (proc == NULL) {
        return NULL;
    }

    // find redirection indices to set corresponsing file names
    int in_redir_index = find_redir_index(parsed_input, "<");
    int out_redir_index = find_redir_index(parsed_input, ">");
    int err_redir_index = find_redir_index(parsed_input, ">>");

    if (in_redir_index != -1 && in_redir_index > start_index && in_redir_index < end_index - 1) {
        in_file = parsed_input[in_redir_index + 1];
    }
    if (out_redir_index != -1 && out_redir_index > start_index && out_redir_index < end_index - 1) {
        out_file = parsed_input[out_redir_index + 1];
    }
    if (err_redir_index != -1 && err_redir_index > start_index && err_redir_index < end_index - 1) {
        err_file = parsed_input[err_redir_index + 1];
    }

    // set the process struct fields
    proc->in_file = in_file;
    proc->out_file = out_file;
    proc->err_file = err_file;
}
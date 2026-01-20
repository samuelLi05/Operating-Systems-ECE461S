#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

// Parses the input string into an array of strings (tokens) split by spaces
// Returns a NULL-terminated array of strings
char** string_parser(char* input_string)
{
    if (input_string == NULL) {
        return NULL;
    }
    
    // Allocate memeory for array of string pointers, realloc as needed
    uint32_t array_size = 10; // initial size
    char** string_array = malloc(array_size * sizeof(char*));
    if (string_array == NULL) {
        return NULL;
    }

    uint32_t index = 0;
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
/**
 Copyright 2018 Igor Efremov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial portions
 of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _usage(FILE *dest) {
    fprintf(dest, "usage: brainfuck <input file>\n");
    fflush(dest);
}

static void _increment_pointer(char **ptr) { *ptr = *ptr + 1; }
static void _decrement_pointer(char **ptr) { *ptr = *ptr - 1; }

static void _increment_value(char **ptr) { **ptr = (**ptr) + 1; }
static void _decrement_value(char **ptr) { **ptr = (**ptr) - 1; }

static void _put_char(char **ptr) { fputc(**ptr, stdout); }
static void _get_char(char **ptr) { **ptr = fgetc(stdin); }

static void _start_while(char **ptr) { }
static void _end_while(char **ptr) { }

static uint32_t _execute_stack(char **ptr, void(**stack)(char **), uint32_t stack_index, uint32_t length) {
    uint32_t index = stack_index;
    void(*next_func)(char **);

    for(; index < length; index++) {
        next_func = stack[index];

        if(next_func == _start_while) {
            uint32_t end_loop_index;
            while(**ptr) {
                end_loop_index = _execute_stack(ptr, stack, index + 1, length);
            }
            
            index = end_loop_index;
            if(index >= length) {
                return index;
            }
            next_func = stack[index];
        } else if(next_func == _end_while) {
            return index + 1;
        }

        (*next_func)(ptr);
    }

    return index;
}

int main(int nargs, char **argv) {
    if(nargs != 2) {
        _usage(stderr);
        return 1;
    }

    FILE *input_file;
    if(!(input_file = fopen(argv[1], "r"))) {
        fprintf(stderr, "unable to open \"%s\"\n", argv[1]);
        fflush(stderr);
        return 2;
    }

    char array[30000];
    void(*execution_stack[30000])(char **);
    memset(array, 0, 30000);

    char c;
    uint32_t stack_index = 0;

    //TODO: Use a dynamically allocated execution stack
    while((c = fgetc(input_file)) != EOF) {
        switch(c) {
            case '>':
                execution_stack[stack_index] = &_increment_pointer;
                break;

            case '<':
                execution_stack[stack_index] = &_decrement_pointer;
                break;

            case '+':
                execution_stack[stack_index] = &_increment_value;
                break;

            case '-':
                execution_stack[stack_index] = &_decrement_value;
                break;

            case '.':
                execution_stack[stack_index] = &_put_char;
                break;

            case ',':
                execution_stack[stack_index] = &_get_char;
                break;

            case '[':
                execution_stack[stack_index] = &_start_while;
                break;

            case ']':
                execution_stack[stack_index] = &_end_while;
                break;

            default:
                //TODO: Something with unknowns
                continue;
        }

        stack_index++;
    }

    char *ptr = array;
    _execute_stack(&ptr, execution_stack, 0, stack_index);

    fclose(input_file);
    return 0;
}

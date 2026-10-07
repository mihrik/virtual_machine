#include "stack.h"
#include <string.h>

const int MAX_COMMAND_SIZE = 40;


typedef enum commands
{
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    DIV = 4,
    OUT = 5,
    HLT = 0,
    UC= 666
} comands;


#define COMMAND_CMP(to_check, reference) {\
     if (strcmp(to_check, #reference) == 0)\
        return reference;\
}

error_codes args_analysis(int argc, const char * const argv[], FILE **fp);
commands get_command(const char *command);
stack_elem_t eval(stack_t *stack, FILE *fp);
#include "stack.h"
#include <string.h>

const int MAX_COMMAND_SIZE = 40;
const int MAX_BUFFER_SIZE = 200;

typedef enum commands
{
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    DIV = 4,
    OUT = 5,
    POPr = 6,
    PUSHr = 7,
    MUL = 8,
    JMP = 9,
    HLT = 0xF4,
    UC= 666
} comands;


typedef enum registers
{
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
    UR = 666
} registers;

typedef struct buf_data
{
    size_t char_num;
    size_t strings_num;
} buf_data;

typedef struct vm_t
{
    stack_t stack;
    stack_elem_t *code;
    size_t ip;
    stack_elem_t registers[5];
} vm_t;

#define CHECK_ERROR(message) {\
    PRINT_COLOR(EXTRA_RED, #message " WHILE DOING %s:%d in function: %s", __FILE__, __LINE__, __func__);\
    free(code);\
    return POISON;\
}

#define COMMAND_CMP(to_check, reference) {\
     if (strcmp(to_check, #reference) == 0)\
        return reference;\
}

#define REGISTER_CMP(to_check, reference) {\
     if (strcmp(to_check, #reference) == 0)\
        return reference;\
}

error_codes args_analysis(int argc, const char * const argv[], FILE **fp);
commands get_command(const char *command);
stack_elem_t eval(vm_t *stack, FILE *fp);
stack_elem_t * read_text(FILE *fp);
long get_file_size(FILE *fp);
registers get_register(const char *reg);
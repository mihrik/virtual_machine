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
    JA = 10,
    JAE = 11,
    JB = 12,
    JBE = 13,
    JE = 14,
    JNE = 15,
    IN = 16,
    CALL = 17,
    RET = 18,
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
    stack_t funcs;
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

#define CHECK_MARK(line) {\
    if (line[0] == ':')\
    {\
        int mark = 0;\
        sscanf(line, ":%d", &mark);\
        labels[mark] = (int)*ip;\
        continue;\
    }\
}

#define CHECK_COMMAND(command) {\
    if (command == UC)\
    {\
        printf("GOT_UNKNOWN_COMMAND: %s, WHILE DOING: %s:%d in function: %s\n", command_line, __FILE__, __LINE__, __func__);\
        return UNKNOWN_COMMAND;\
    }\
}

error_codes args_analysis(int argc, const char * const argv[], FILE **fp);
commands get_command(const char *command);
stack_elem_t eval(vm_t *stack, FILE *fp);
stack_elem_t * read_text(FILE *fp);
long get_file_size(FILE *fp);
registers get_register(const char *reg);
error_codes vm_push(vm_t *vm, stack_elem_t *code);
error_codes vm_add(vm_t *vm);
error_codes vm_sub(vm_t *vm);
error_codes vm_div(vm_t *vm);
error_codes vm_mul(vm_t *vm);
error_codes vm_popr(vm_t *vm, stack_elem_t *code);
error_codes vm_pushr(vm_t *vm, stack_elem_t *code);
error_codes vm_jmp(vm_t *vm, stack_elem_t *code);
error_codes vm_out(vm_t *vm, stack_elem_t *res);
error_codes vm_hlt(vm_t *vm, stack_elem_t *code);
void ignore_comment(char line[]);
error_codes asm_push(char line[], size_t *ip, stack_elem_t buffer[]);
error_codes asm_reg_funcs(char line[], size_t *ip, stack_elem_t buffer[]);
error_codes asm_jmp(char line[], size_t *ip, stack_elem_t buffer[], int labels[]);
error_codes command_analysis(char line[], commands command, size_t *ip, stack_elem_t buffer[], int labels[]);
error_codes assemble(FILE *fp_r, stack_elem_t **code, size_t *ip);
error_codes vm_ja(vm_t *vm, stack_elem_t *code);
error_codes vm_jae(vm_t *vm, stack_elem_t *code);
error_codes vm_jb(vm_t *vm, stack_elem_t *code);
error_codes vm_jbe(vm_t *vm, stack_elem_t *code);
error_codes vm_je(vm_t *vm, stack_elem_t *code);
error_codes vm_jne(vm_t *vm, stack_elem_t *code);
error_codes vm_in(vm_t *vm);
error_codes vm_call(vm_t *vm, stack_elem_t *code);
error_codes vm_ret(vm_t *vm);
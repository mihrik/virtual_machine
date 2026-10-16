#include "stack.h"
#include <string.h>

const int MAX_COMMAND_SIZE = 400;
const int MAX_BUFFER_SIZE = 2000;

typedef enum commands
{
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_DIV = 4,
    CMD_OUT = 5,
    CMD_POPr = 6,
    CMD_PUSHr = 7,
    CMD_MUL = 8,
    CMD_JMP = 9,
    CMD_JA = 10,
    CMD_JAE = 11,
    CMD_JB = 12,
    CMD_JBE = 13,
    CMD_JE = 14,
    CMD_JNE = 15,
    CMD_IN = 16,
    CMD_CALL = 17,
    CMD_RET = 18,
    CMD_SQRT = 19,
    CMD_HLT = 0xF4,
    CMD_UC= 666
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
    stack_elem_t registers[20];
} vm_t;

typedef struct asm_t
{
    char line[MAX_COMMAND_SIZE];
    char command_line[MAX_COMMAND_SIZE];
    stack_elem_t *buffer;
    commands command;
    size_t ip;
    int labels[20];

} asm_t;

#define CHECK_ERROR(message) {\
    PRINT_COLOR(EXTRA_RED, #message " WHILE DOING %s:%d in function: %s", __FILE__, __LINE__, __func__);\
    free(code);\
    return POISON;\
}

#define CHECK_MARK(line) {\
    if ((line)[0] == ':')\
    {\
        int mark = 0;\
        sscanf((line), ":%d", &mark);\
        assembler->labels[mark] = (int)assembler->ip;\
        continue;\
    }\
}

#define SKIP_STRING(line) {\
    if ((line)[0] == '\0' || (line)[0] == '\n')\
        continue;\
}

#define CHECK_COMMAND(command) {\
    if ((command) == CMD_UC)\
    {\
        printf("GOT_UNKNOWN_COMMAND: %s, WHILE DOING: %s:%d in function: %s\n", assembler->command_line, __FILE__, __LINE__, __func__);\
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
error_codes vm_popr(vm_t *vm, stack_elem_t *code);
error_codes vm_pushr(vm_t *vm, stack_elem_t *code);
error_codes vm_jmp(vm_t *vm, stack_elem_t *code);
error_codes vm_out(vm_t *vm, stack_elem_t *res);
error_codes vm_hlt(vm_t *vm, stack_elem_t *code);
error_codes ignore_comment(char line[]);
error_codes asm_push(asm_t *assembler);
error_codes asm_reg_funcs(asm_t *assembler);
error_codes asm_jmp(asm_t *assembler);
error_codes command_analysis(asm_t *assembler);
error_codes assemble(FILE *fp_r, asm_t *assembler);
error_codes vm_in(vm_t *vm);
error_codes vm_call(vm_t *vm, stack_elem_t *code);
error_codes vm_ret(vm_t *vm);
error_codes vm_ctor(vm_t *vm, FILE *fp);
error_codes asm_ctor(asm_t *assembler);
error_codes pass(FILE *fp_r, asm_t *assembler);
error_codes vm_sqrt(vm_t *vm);
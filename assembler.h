#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include "enum.h"

const int MAX_COMMAND_SIZE = 400;
const int MAX_BUFFER_SIZE = 2000;
const int MAX_MARK_NAME_SIZE = 20;
const int MAX_COMMAND_NAME_LEN = 10;
const int MAX_REGISTER_NAME_LEN = 3;

#define MODE_MRK 225
#define MODE_MR 193
#define MODE_MK 161
#define MODE_RK 97
#define MODE_R 65
#define MODE_K 33

typedef struct mark_t
{
    char name[MAX_MARK_NAME_SIZE];
    size_t new_ip;
} mark_t;

typedef struct asm_t
{
    char line[MAX_COMMAND_SIZE];
    char command_line[MAX_COMMAND_SIZE];
    stack_elem_t *buffer;
    commands command;
    size_t ip;
    mark_t labels[20];
} asm_t;

typedef struct data_hash
{
    int num;
    size_t hash;
    char name[MAX_COMMAND_NAME_LEN];
} data_hash;

#define CHECK_ERROR(message) {\
    PRINT_COLOR(EXTRA_RED, #message " WHILE DOING %s:%d in function: %s", __FILE__, __LINE__, __func__);\
    free(code);\
    return POISON;\
}

#define CHECK_MARK(line) {\
    if ((line)[0] == ':')\
    {\
        for (size_t i = 0; i < sizeof(assembler->labels) / sizeof(assembler->labels[0]); i++)\
        {\
            if (assembler->labels[i].name[0] == '\0')\
            {\
                sscanf((line), ":%s", assembler->labels[i].name);\
                assembler->labels[i].new_ip = assembler->ip;\
                break;\
            }\
        }\
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

#define PUSH_KONST(command) {\
    one_argument(assembler, elem, (command));\
}

#define PUSH_KONST_AND_REG(command) {\
    two_arguments(assembler, elem, rnum, (command));\
}

#define PUSH_REG(command) {\
    one_argument(assembler, rnum, (command));\
}

error_codes asm_push(asm_t *assembler);
error_codes asm_reg_funcs(asm_t *assembler);
error_codes asm_jmp(asm_t *assembler);
error_codes command_analysis(asm_t *assembler);
error_codes assemble(FILE *fp_r, asm_t *assembler);
error_codes asm_ctor(asm_t *assembler);
error_codes pass(FILE *fp_r, asm_t *assembler);
error_codes asm_ram(asm_t *assembler);
commands get_command(const char *command);
registers get_register(const char *reg);
error_codes ignore_comment(char line[]);
error_codes two_arguments(asm_t *assembler, stack_elem_t elem, stack_elem_t rnum, stack_elem_t command);
error_codes one_argument(asm_t *assembler, stack_elem_t rnum, stack_elem_t command);
error_codes memory_off_push(asm_t *assembler, char specifier[]);
error_codes memory_on_push(asm_t *assembler, char specifier[]);
size_t djb2_for_asm(const char name[]);
size_t hash_binary_search(size_t hash, data_hash to_sort[], size_t arr_len);
int compare_hashes_up(const void *val1, const void *val2);
void sort_hashes_and_regs(void);

#endif
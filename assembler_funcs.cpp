#include "assembler.h"
#include <string.h>

data_hash cmd_hashes[] =
{
    {.num = CMD_SQRT, .hash = djb2_for_asm("SQRT"), .name = "SQRT"},
    {.num = CMD_CALL, .hash = djb2_for_asm("CALL"), .name = "CALL"},
    {.num = CMD_DRAW, .hash = djb2_for_asm("DRAW"), .name = "DRAW"},
    {.num = CMD_PUSH, .hash = djb2_for_asm("PUSH"), .name = "PUSH"},
    {.num = CMD_ADD, .hash = djb2_for_asm("ADD"), .name = "ADD"},
    {.num = CMD_JMP, .hash = djb2_for_asm("JMP"), .name = "JMP"},
    {.num = CMD_JNE, .hash = djb2_for_asm("JNE"), .name = "JNE"},
    {.num = CMD_SUB, .hash = djb2_for_asm("SUB"), .name = "SUB"},
    {.num = CMD_DIV, .hash = djb2_for_asm("DIV"), .name = "DIV"},
    {.num = CMD_RET, .hash = djb2_for_asm("RET"), .name = "RET"},
    {.num = CMD_MUL, .hash = djb2_for_asm("MUL"), .name = "MUL"},
    {.num = CMD_JAE, .hash = djb2_for_asm("JAE"), .name = "JAE"},
    {.num = CMD_JBE, .hash = djb2_for_asm("JBE"), .name = "JBE"},
    {.num = CMD_OUT, .hash = djb2_for_asm("OUT"), .name = "OUT"},
    {.num = CMD_HLT, .hash = djb2_for_asm("HLT"), .name = "HLT"},
    {.num = CMD_POP, .hash = djb2_for_asm("POP"), .name = "POP"},
    {.num = CMD_IN, .hash = djb2_for_asm("IN"), .name = "IN"},
    {.num = CMD_JA, .hash = djb2_for_asm("JA"), .name = "JA"},
    {.num = CMD_JB, .hash = djb2_for_asm("JB"), .name = "JB"},
    {.num = CMD_JE, .hash = djb2_for_asm("JE"), .name = "JE"}
};

data_hash r_hashes[]=
{
    {.num = AX, .hash = djb2_for_asm("AX"), .name = "AX"},
    {.num = BX, .hash = djb2_for_asm("BX"), .name = "BX"},
    {.num = CX, .hash = djb2_for_asm("CX"), .name = "CX"},
    {.num = DX, .hash = djb2_for_asm("DX"), .name = "DX"},
    {.num = EX, .hash = djb2_for_asm("EX"), .name = "EX"},
    {.num = FX, .hash = djb2_for_asm("FX"), .name = "FX"},
    {.num = GX, .hash = djb2_for_asm("GX"), .name = "GX"},
    {.num = HX, .hash = djb2_for_asm("HX"), .name = "HX"},
    {.num = IX, .hash = djb2_for_asm("IX"), .name = "IX"}
};

error_codes ignore_comment(char line[])
{
    char *semicolon_ptr = NULL;
    if ((semicolon_ptr = strchr(line, ';')) != NULL)
        *semicolon_ptr = '\0';

    return SUCCESSFUL_RETURN;
}

error_codes asm_variable(asm_t *assembler, commands cmd)
{
    assert(assembler);

    char specifier[MAX_COMMAND_SIZE] = {};
    size_t command = cmd;
    sscanf(assembler->line, "%*s %[^\n]", specifier);
    error_codes err = SUCCESSFUL_RETURN;

    char* argument = specifier;

    if (specifier[0] == '[')
    {
        command = command | MODE_M;
        argument++;
    }

    err = execute_variable(assembler, argument, command);

    return err;
}

error_codes asm_reg_funcs(asm_t *assembler)
{
    assert(assembler);

    char reg[MAX_COMMAND_SIZE] = {};
    sscanf(assembler->line, "%*s %s", reg);

    stack_elem_t rnum = get_register(reg);
    assembler->buffer[assembler->ip] = rnum;
    assembler->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes asm_jmp(asm_t *assembler)
{
    assert(assembler);

    char name[MAX_MARK_NAME_SIZE] = {};

    if (sscanf(assembler->line, "%*s %s", name) != 1)
        return  MARK_ABSENCE;
    for (size_t i = 0; i < sizeof(assembler->labels) / sizeof(assembler->labels[0]); i++)
    {
        if (strcmp(name, assembler->labels[i].name) == 0)
        {
            assembler->buffer[assembler->ip] = (stack_elem_t) assembler->labels[i].new_ip;
            break;
        }
    }
    assembler->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes command_analysis(asm_t *assembler)
{
    assert(assembler);

    error_codes err = SUCCESSFUL_RETURN;

    switch(assembler->command)
    {
        case CMD_PUSH :
                        {
                            err = asm_variable(assembler, CMD_PUSH);

                            break;
                        }
        case CMD_POP :
                        {
                            err = asm_variable(assembler, CMD_POP);

                            break;
                        }
        case CMD_JMP  :
        case CMD_JA   :
        case CMD_JAE  :
        case CMD_JB   :
        case CMD_JBE  :
        case CMD_JE   :
        case CMD_JNE  :
        case CMD_CALL :
                        {
                            asm_jmp(assembler);
                        }
        default   : break;

    }

    return err;
}

error_codes assemble(FILE *fp_r, asm_t *assembler)
{
    assert(assembler);
    assert(fp_r);

    error_codes err = SUCCESSFUL_RETURN;

    sort_hashes_and_regs();

    for (size_t i = 0; i < 2; i++)
    {
        err = pass(fp_r, assembler);
    }

    return err;
}

error_codes pass(FILE *fp_r, asm_t *assembler)
{
    assert(fp_r);
    assert(assembler);

    assembler->ip = 0;
    rewind(fp_r);
    error_codes err = SUCCESSFUL_RETURN;

    while (fgets(assembler->line, MAX_COMMAND_SIZE, fp_r))
    {
        ignore_comment(assembler->line);

        SKIP_STRING(assembler->line);

        CHECK_MARK(assembler->line);

        if (sscanf(assembler->line, "%s", assembler->command_line) != 1)
        {
            printf("ABSENCE OF THE COMMAND NUMBER" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
            return INVALID_QUANTITY_OF_ARGUMENTS;
        }

        assembler->command = get_command(assembler->command_line);
        CHECK_COMMAND(assembler->command);

        assembler->buffer[assembler->ip] = assembler->command;
        assembler->ip += 1;

        err = command_analysis(assembler);
    }

    return err;
}

error_codes asm_ctor(asm_t *assembler)
{
    assert(assembler);

    assembler->buffer = (stack_elem_t *) calloc(MAX_BUFFER_SIZE, sizeof(stack_elem_t));

    if (assembler->buffer == NULL)
        return MEMORY_ALLOCATION_ERROR;

    return SUCCESSFUL_RETURN;
}

error_codes two_arguments(asm_t *assembler, stack_elem_t elem, stack_elem_t rnum, stack_elem_t command)
{
    assert(assembler);

    assembler->buffer[assembler->ip - 1] = command;
    assembler->buffer[assembler->ip++] = elem;
    assembler->buffer[assembler->ip++] = rnum;

    return SUCCESSFUL_RETURN;
}

error_codes one_argument(asm_t *assembler, stack_elem_t argument, stack_elem_t command)
{
    assert(assembler);

    assembler->buffer[assembler->ip - 1] = command;
    assembler->buffer[assembler->ip++] = argument;

    return SUCCESSFUL_RETURN;
}

error_codes execute_variable(asm_t *assembler, char specifier[], size_t command)
{
    assert(assembler);

    char reg[MAX_COMMAND_SIZE] = "";
    stack_elem_t elem = 0;
    int pos = -1;

    bool is_push   = ((command & 0x1F)   == CMD_PUSH);
    bool is_pop    = ((command & 0x1F)   == CMD_POP);
    bool is_memory = ((command & MODE_M) != 0);

    bool allow_constant = is_push || (is_pop && is_memory);

    if (sscanf(specifier, " %1[A-I]X%n", reg, &pos) == 1 && pos != -1)
    {
        reg[1] = 'X';

        stack_elem_t rnum = get_register(reg);
        command = command | MODE_R;
        if (sscanf(specifier, " %*[^+] + " deb_spec, &elem) == 1)
        {
            if (!allow_constant)
                return WRONG_POP_PARAMETERS;

            command = command | MODE_K;
            PUSH_KONST_AND_REG(command);
            return SUCCESSFUL_RETURN;
        }
        else
        {
            command = command | MODE_R;
            PUSH_REG(command);
            return SUCCESSFUL_RETURN;
        }
    }
    else if (allow_constant)
    {
        if (sscanf(specifier, deb_spec, &elem) != 1)
            return INVALID_QUANTITY_OF_ARGUMENTS;
        command = command | MODE_K;
        PUSH_KONST(command);
        return SUCCESSFUL_RETURN;
    }

    return WRONG_POP_PARAMETERS;
}

size_t djb2_for_asm(const char name[])
{
    size_t sum = 5381;

    for (size_t i = 0; i < MAX_COMMAND_NAME_LEN && name[i]; i++)
    {
        sum = sum * 33 + (size_t)name[i];
    }

    return sum % 1000;
}

commands get_command(const char *command)
{
    size_t hash = djb2_for_asm(command);

    size_t index = hash_binary_search(hash, cmd_hashes, sizeof(cmd_hashes) / sizeof(cmd_hashes[0]));
    if (strcmp(command, cmd_hashes[index].name) == 0)
    {
        return (commands)cmd_hashes[index].num;
    }

    return CMD_UC;
}

size_t hash_binary_search(size_t hash, const data_hash table[], size_t arr_len)
{
    assert(table);

    size_t l = 0, r = arr_len - 1;
    size_t mid = l + (r - l) / 2;

    while (l < r)
    {
        if (hash <= table[mid].hash)
        {
            r = mid;
        }
        else
        {
            l = mid + 1;
        }

        mid = l + (r - l) / 2;
    }

    return r;
}

registers get_register(const char *reg)
{
    size_t hash = djb2_for_asm(reg);

    size_t index = hash_binary_search(hash, r_hashes, sizeof(r_hashes) / sizeof(r_hashes[0]));
    if (strcmp(reg, r_hashes[index].name) == 0)
    {
        return (registers)r_hashes[index].num;
    }

    return UR;
}

int compare_hashes_up(const void *val1, const void *val2)
{
    assert(val1);
    assert(val2);

    const data_hash num1 = *(const data_hash *)val1;
    const data_hash num2 = *(const data_hash *)val2;

    return (int)num1.hash - (int)num2.hash;
}

void sort_hashes_and_regs(void)
{
    qsort(cmd_hashes, sizeof(cmd_hashes) / sizeof(cmd_hashes[0]), sizeof(cmd_hashes[0]), compare_hashes_up);
    qsort(r_hashes, sizeof(r_hashes) / sizeof(r_hashes[0]), sizeof(r_hashes[0]), compare_hashes_up);
}

void put_signature(asm_t *assembler, FILE *fp_w)
{
    fwrite(MESSAGE, sizeof(char), MESSAGE_SIZE, fp_w);
}
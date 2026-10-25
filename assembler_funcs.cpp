#include "assembler.h"
#include <string.h>
// TODO review neighbour's code and mentors

data_hash cmd_hashes[] =
{
    {.num = CMD_POPmr, .hash = djb2_for_asm("POPmr"), .name = "POPmr"},
    {.num = CMD_SQRT, .hash = djb2_for_asm("SQRT"), .name = "SQRT"},
    {.num = CMD_CALL, .hash = djb2_for_asm("CALL"), .name = "CALL"},
    {.num = CMD_POPm, .hash = djb2_for_asm("POPm"), .name = "POPm"},
    {.num = CMD_POPr, .hash = djb2_for_asm("POPr"), .name = "POPr"},
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

error_codes asm_push(asm_t *assembler)
{
    assert(assembler);

    char specifier[MAX_COMMAND_SIZE] = {};
    sscanf(assembler->line, "%*s %[^\n]", specifier);

    if (specifier[0] == '[')
    {
        memory_on_push(assembler, specifier);
    }
    else
    {
        memory_off_push(assembler, specifier);
    }

    return SUCCESSFUL_RETURN;
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

    sscanf(assembler->line, "%*s %s", name);
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

    switch(assembler->command)
    {
        case CMD_PUSH :
                        {
                            asm_push(assembler);

                            break;
                        }
        case CMD_POPr :
        case CMD_POPmr:
                        {
                            asm_reg_funcs(assembler);

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

                            break;
                        }
        case CMD_POPm :
                        {
                            asm_ram(assembler);

                            break;
                        }
        default   : break;

    }

    return SUCCESSFUL_RETURN;
}

error_codes assemble(FILE *fp_r, asm_t *assembler)
{
    assert(assembler);
    assert(fp_r);

    sort_hashes_and_regs();

    for (size_t i = 0; i < 2; i++)
    {
        pass(fp_r, assembler);
    }

    return SUCCESSFUL_RETURN;
}

error_codes pass(FILE *fp_r, asm_t *assembler)
{
    assert(fp_r);
    assert(assembler);

    assembler->ip = 0;
    rewind(fp_r);

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

        command_analysis(assembler);
    }

    return SUCCESSFUL_RETURN;
}

error_codes asm_ctor(asm_t *assembler)
{
    assert(assembler);

    assembler->buffer = (stack_elem_t *) calloc(MAX_BUFFER_SIZE, sizeof(stack_elem_t));

    if (assembler->buffer == NULL)
        return MEMORY_ALLOCATION_ERROR;

    return SUCCESSFUL_RETURN;
}

error_codes asm_ram(asm_t *assembler)
{
    assert(assembler);

    stack_elem_t elem = 0;

    if (sscanf(assembler->line, "%*s" "%*c" deb_spec, &elem) != 1)
    {
        printf("ABSENCE OF THE SECOND PARAMETER OF PUSH" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
        return INVALID_QUANTITY_OF_ARGUMENTS;
    }

    assembler->buffer[assembler->ip] = elem;
    assembler->ip += 1;

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

error_codes memory_off_push(asm_t *assembler, char specifier[])
{
    assert(assembler);

    char reg[MAX_COMMAND_SIZE] = {};
    stack_elem_t elem = 0;
    if (sscanf(specifier, "%[A-IX]", reg) == 1)
    {
        stack_elem_t rnum = get_register(reg);
        if (sscanf(specifier, "%*[^+]+ " deb_spec, &elem) == 1)
        {
            PUSH_KONST_AND_REG(MODE_RK);
            return SUCCESSFUL_RETURN;
        }
        else
        {
            PUSH_REG(MODE_R);
            return SUCCESSFUL_RETURN;
        }
    }
    else
    {
            sscanf(specifier, deb_spec, &elem);
            PUSH_KONST(MODE_K);
            return SUCCESSFUL_RETURN;
    }

    return SUCCESSFUL_RETURN;
}

error_codes memory_on_push(asm_t *assembler, char specifier[])
{
    assert(assembler);

    char reg[MAX_COMMAND_SIZE] = {};
    stack_elem_t elem = 0;
    if (sscanf(specifier, "[%[A-IX]", reg) == 1)
    {
        stack_elem_t rnum = get_register(reg);
        if (sscanf(specifier, "%*[^+]+ " deb_spec, &elem) == 1)
        {
            PUSH_KONST_AND_REG(MODE_MRK);
            return SUCCESSFUL_RETURN;
        }
        else
        {
            PUSH_REG(MODE_MR);
            return SUCCESSFUL_RETURN;
        }
    }
    else
    {
        sscanf(specifier, "[" deb_spec, &elem);
        PUSH_KONST(MODE_MK);
        return SUCCESSFUL_RETURN;
    }

    return SUCCESSFUL_RETURN;
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
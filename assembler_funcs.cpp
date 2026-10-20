#include "assembler.h"
#include <string.h>
// TODO review neighbour's code and mentors

command_hash hashes[] =
{
    {.cnum = CMD_CALL, .hash = 49, .name = "CALL"},
    {.cnum = CMD_ADD, .hash = 94, .name = "ADD"},
    {.cnum = CMD_DRAW, .hash = 147, .name = "DRAW"},
    {.cnum = CMD_JMP, .hash = 204, .name = "JMP"},
    {.cnum = CMD_JNE, .hash = 226, .name = "JNE"},
    {.cnum = CMD_PUSH, .hash = 237, .name = "PUSH"},
    {.cnum = CMD_SUB, .hash = 255, .name = "SUB"},
    {.cnum = CMD_POPmr, .hash = 267, .name = "POPmr"},
    {.cnum = CMD_IN, .hash = 396, .name = "IN"},
    {.cnum = CMD_JA, .hash = 416, .name = "JA"},
    {.cnum = CMD_JB, .hash = 417, .name = "JB"},
    {.cnum = CMD_JE, .hash = 420, .name = "JE"},
    {.cnum = CMD_DIV, .hash = 544, .name = "DIV"},
    {.cnum = CMD_POPm, .hash = 641, .name = "POPm"},
    {.cnum = CMD_POPr, .hash = 646, .name = "POPr"},
    {.cnum = CMD_RET, .hash = 656, .name = "RET"},
    {.cnum = CMD_SQRT, .hash = 671, .name = "SQRT"},
    {.cnum = CMD_MUL, .hash = 731, .name = "MUL"},
    {.cnum = CMD_JAE, .hash = 797, .name = "JAE"},
    {.cnum = CMD_JBE, .hash = 830, .name = "JBE"},
    {.cnum = CMD_OUT, .hash = 917, .name = "OUT"},
    {.cnum = CMD_HLT, .hash = 997, .name = "HLT"},
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

#define REGISTER_CMP_(to_check, reference) {\
     if (strcmp(to_check, #reference) == 0)\
        return reference;\
}

registers get_register(const char *reg)
{
    REGISTER_CMP_(reg, AX);
    REGISTER_CMP_(reg, BX);
    REGISTER_CMP_(reg, CX);
    REGISTER_CMP_(reg, DX);
    REGISTER_CMP_(reg, EX);
    REGISTER_CMP_(reg, FX);
    REGISTER_CMP_(reg, GX);
    REGISTER_CMP_(reg, HX);
    REGISTER_CMP_(reg, IX);

    return UR;
}

#undef REGISTER_CMP_

error_codes asm_reg_funcs(asm_t *assembler)
{
    char reg[MAX_COMMAND_SIZE] = {};
    sscanf(assembler->line, "%*s %s", reg);

    stack_elem_t rnum = get_register(reg);
    assembler->buffer[assembler->ip] = rnum;
    assembler->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes asm_jmp(asm_t *assembler)
{
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
    for (size_t i = 0; i < 2; i++)
    {
        pass(fp_r, assembler);
    }

    return SUCCESSFUL_RETURN;
}

error_codes pass(FILE *fp_r, asm_t *assembler)
{
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
    assembler->buffer = (stack_elem_t *) calloc(MAX_BUFFER_SIZE, sizeof(stack_elem_t));

    if (assembler->buffer == NULL)
        return MEMORY_ALLOCATION_ERROR;

    return SUCCESSFUL_RETURN;
}

error_codes asm_ram(asm_t *assembler)
{
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
    assembler->buffer[assembler->ip - 1] = command;
    assembler->buffer[assembler->ip++] = elem;
    assembler->buffer[assembler->ip++] = rnum;

    return SUCCESSFUL_RETURN;
}

error_codes one_argument(asm_t *assembler, stack_elem_t argument, stack_elem_t command)
{
    assembler->buffer[assembler->ip - 1] = command;
    assembler->buffer[assembler->ip++] = argument;

    return SUCCESSFUL_RETURN;
}

error_codes memory_off_push(asm_t *assembler, char specifier[])
{
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
        sscanf(specifier, deb_spec, &elem);
        PUSH_KONST(MODE_MK);
        return SUCCESSFUL_RETURN;
    }

    return SUCCESSFUL_RETURN;
}

size_t djb2_for_asm(const char name[])
{
    size_t sum = 5381;

    for (size_t i = 0; i < MAX_NAME_LEN && name[i]; i++)
    {
        sum = sum * 33 + (size_t)name[i];
    }

    return sum % 1000;
}

commands get_command(const char *command)
{
    size_t hash = djb2_for_asm(command);

    size_t index = binary_search(hash);
    if (strcmp(command, hashes[index].name) == 0)
    {
        return hashes[index].cnum;
    }

    return CMD_UC;
}

size_t binary_search(size_t hash)
{
    size_t l = 0, r = sizeof(hashes) / sizeof(hashes[0]) - 1;
    size_t mid = l + (r - l) / 2;

    while (l < r)
    {
        if (hash <= hashes[mid].hash)
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
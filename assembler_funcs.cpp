#include "vm.h"
#include <string.h>
// TODO review neighbour's code and mentors
#define COMMAND_CMP_(to_check, reference) {\
     if (strcmp(to_check, #reference) == 0)\
        return reference;\
}

commands get_command(const char *command)
{
    assert(command);

    char to_check[MAX_COMMAND_SIZE * 2] = "CMD_";
    strncat(to_check, command, MAX_COMMAND_SIZE);

    COMMAND_CMP_(to_check, CMD_PUSH);
    COMMAND_CMP_(to_check, CMD_ADD);
    COMMAND_CMP_(to_check, CMD_SUB);
    COMMAND_CMP_(to_check, CMD_DIV);
    COMMAND_CMP_(to_check, CMD_OUT);
    COMMAND_CMP_(to_check, CMD_HLT);
    COMMAND_CMP_(to_check, CMD_POPr);
    COMMAND_CMP_(to_check, CMD_MUL);
    COMMAND_CMP_(to_check, CMD_JMP);
    COMMAND_CMP_(to_check, CMD_JA);
    COMMAND_CMP_(to_check, CMD_JAE);
    COMMAND_CMP_(to_check, CMD_JB);
    COMMAND_CMP_(to_check, CMD_JBE);
    COMMAND_CMP_(to_check, CMD_JE);
    COMMAND_CMP_(to_check, CMD_JNE);
    COMMAND_CMP_(to_check, CMD_IN);
    COMMAND_CMP_(to_check, CMD_RET);
    COMMAND_CMP_(to_check, CMD_CALL);
    COMMAND_CMP_(to_check, CMD_SQRT);
    COMMAND_CMP_(to_check, CMD_POPm);
    COMMAND_CMP_(to_check, CMD_DRAW);
    COMMAND_CMP_(to_check, CMD_POPmr);

    return CMD_UC;
}

#undef COMMAND_CMP_

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
        char reg[MAX_COMMAND_SIZE] = {};
        stack_elem_t elem = 0;
        if (sscanf(specifier, "[%[A-IX]", reg) == 1)
        {
            stack_elem_t rnum = get_register(reg);
            if (sscanf(specifier, "%*[^+]+ " deb_spec, &elem) == 1)
            {
                assembler->buffer[assembler->ip - 1] = 225;
                assembler->buffer[assembler->ip++] = elem;
                assembler->buffer[assembler->ip++] = rnum;
                return SUCCESSFUL_RETURN;
            }
            else
            {
                assembler->buffer[assembler->ip - 1] = 193;
                assembler->buffer[assembler->ip++] = rnum;
                return SUCCESSFUL_RETURN;
            }
        }
        else
        {
                sscanf(specifier, "[" deb_spec, &elem);
                assembler->buffer[assembler->ip - 1] = 161;
                assembler->buffer[assembler->ip++] = elem;
                return SUCCESSFUL_RETURN;
        }

    }
    else
    {
        char reg[MAX_COMMAND_SIZE] = {};
        stack_elem_t elem = 0;
        if (sscanf(specifier, "%[A-IX]", reg) == 1)
        {
            stack_elem_t rnum = get_register(reg);
            if (sscanf(specifier, "%*[^+]+ " deb_spec, &elem) == 1)
            {
                assembler->buffer[assembler->ip - 1] = 97;
                assembler->buffer[assembler->ip++] = elem;
                assembler->buffer[assembler->ip++] = rnum;
                return SUCCESSFUL_RETURN;
            }
            else
            {
                assembler->buffer[assembler->ip - 1] = 65;
                assembler->buffer[assembler->ip++] = rnum;
                return SUCCESSFUL_RETURN;
            }
        }
        else
        {
                sscanf(specifier, deb_spec, &elem);
                assembler->buffer[assembler->ip - 1] = 33;
                assembler->buffer[assembler->ip++] = elem;
                return SUCCESSFUL_RETURN;
        }
    }
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
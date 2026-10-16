#include "vm.h"
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
    COMMAND_CMP_(to_check, CMD_PUSHr);
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
    stack_elem_t elem = 0;

    if (sscanf(assembler->line, "%*s" deb_spec, &elem) != 1)
    {
        printf("ABSENCE OF THE SECOND PARAMETER OF PUSH" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
        return INVALID_QUANTITY_OF_ARGUMENTS;
    }

    assembler->buffer[assembler->ip] = elem;
    assembler->ip += 1;

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
    size_t mark = 0;

    sscanf(assembler->line, "%*s %zu", &mark);
    assembler->buffer[assembler->ip] = assembler->labels[mark];
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
        case CMD_PUSHr:
        case CMD_POPr :
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
        if (sscanf(assembler->line, "%s", assembler->command_line) != 1)
        {
            printf("ABSENCE OF THE COMMAND NUMBER" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
            return INVALID_QUANTITY_OF_ARGUMENTS;
        }

        ignore_comment(assembler->line);

        CHECK_MARK(assembler->line);

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
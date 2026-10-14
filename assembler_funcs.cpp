#include "vm.h"

commands get_command(const char *command)
{
    assert(command);

    COMMAND_CMP(command, PUSH);
    COMMAND_CMP(command, ADD);
    COMMAND_CMP(command, SUB);
    COMMAND_CMP(command, DIV);
    COMMAND_CMP(command, OUT);
    COMMAND_CMP(command, HLT);
    COMMAND_CMP(command, PUSHr);
    COMMAND_CMP(command, POPr);
    COMMAND_CMP(command, MUL);
    COMMAND_CMP(command, JMP);

    return UC;
}

void ignore_comment(char line[])
{
    char *semicolon_ptr = NULL;
    if ((semicolon_ptr = strchr(line, ';')) != NULL)
        *semicolon_ptr = '\0';
}

error_codes asm_push(char line[], size_t *ip, stack_elem_t buffer[])
{
    stack_elem_t elem = 0;

    if (sscanf(line, "%*s" deb_spec, &elem) != 1)
    {
        printf("ABSENCE OF THE SECOND PARAMETER OF PUSH" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
        return INVALID_QUANTITY_OF_ARGUMENTS;
    }

    buffer[*ip] = elem;
    *ip += 1;

    return SUCCESSFUL_RETURN;
}

registers get_register(const char *reg)
{
    REGISTER_CMP(reg, AX);
    REGISTER_CMP(reg, BX);
    REGISTER_CMP(reg, CX);
    REGISTER_CMP(reg, DX);

    return UR;
}

error_codes asm_reg_funcs(char line[], size_t *ip, stack_elem_t buffer[])
{
    char reg[MAX_COMMAND_SIZE] = {};
    sscanf(line, "%*s %s", reg);

    stack_elem_t rnum = get_register(reg);
    buffer[*ip] = rnum;
    *ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes asm_jmp(char line[], size_t *ip, stack_elem_t buffer[], int labels[])
{
    size_t mark = 0;

    sscanf(line, "%*s %zu", &mark);
    buffer[*ip] = labels[mark];
    *ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes command_analysis(char line[], commands command, size_t *ip, stack_elem_t buffer[], int labels[])
{
    switch(command)
    {
        case PUSH :
                    {
                        asm_push(line, ip, buffer);

                        break;
                    }
        case PUSHr:
        case POPr :
                    {
                        asm_reg_funcs(line, ip, buffer);

                        break;
                    }
        case JMP  : {
                        asm_jmp(line, ip, buffer, labels);

                        break;
                    }
        default   : break;

    }

    return SUCCESSFUL_RETURN;
}

error_codes assemble(FILE *fp_r, stack_elem_t **code, size_t *ip)
{
    char line[MAX_COMMAND_SIZE] = {};
    char command_line[MAX_COMMAND_SIZE] = {};
    stack_elem_t *buffer = (stack_elem_t *) calloc(MAX_BUFFER_SIZE, sizeof(stack_elem_t));
    commands command = UC;
    int labels[5] = {-1, -1, -1, -1, -1};

    for (size_t i = 0; i < 2; i++)
    {
        *ip = 0;
        rewind(fp_r);

        while (fgets(line, MAX_COMMAND_SIZE, fp_r))
        {
            if (sscanf(line, "%s", command_line) != 1)
            {
                printf("ABSENCE OF THE COMMAND NUMBER" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
                return INVALID_QUANTITY_OF_ARGUMENTS;
            }

            ignore_comment(line);

            CHECK_MARK(line);

            command = get_command(command_line);
            CHECK_COMMAND(command);

            buffer[*ip] = command;
            *ip += 1;

            command_analysis(line, command, ip, buffer, labels);
        }
    }

    *code = buffer;

    return SUCCESSFUL_RETURN;
}
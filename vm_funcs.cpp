#include "vm.h"

error_codes args_analysis(int argc, const char * const argv[], FILE **fp)
{
    if (argc == 1)
    {
        *fp = stdin;
        return SUCCESSFUL_RETURN;
    }
    else if (argc == 2)
    {
        *fp = fopen(argv[1], "r");
        return SUCCESSFUL_RETURN;
    }
    else
    {
        return INVALID_QUANTITY_OF_ARGUMENTS;
    }
}

commands get_command(const char *command)
{
    COMMAND_CMP(command, PUSH);
    COMMAND_CMP(command, ADD);
    COMMAND_CMP(command, SUB);
    COMMAND_CMP(command, DIV);
    COMMAND_CMP(command, OUT);
    COMMAND_CMP(command, HLT);

    return UC;
}

stack_elem_t eval(stack_t *stack, FILE *fp)
{
    char line[MAX_COMMAND_SIZE] = {};
    int tmp = 0;
    commands command = UC;
    stack_elem_t res = 0;

    while(fgets(line, MAX_COMMAND_SIZE, fp))
    {
        sscanf(line, "%d", &tmp);
        command = (commands)tmp;

        switch(command)
        {
            case PUSH:
            {
                stack_elem_t elem = 0;
                sscanf(line, "%*d" deb_spec, &elem);
                STACK_PUSH(stack, elem);
                //stack_dump(stack, "to_check", "PUSH");
                break;
            }
            case ADD:
            {
                stack_elem_t elem1 = 0, elem2 = 0;
                STACK_POP(stack, &elem1);
                STACK_POP(stack, &elem2);
                STACK_PUSH(stack, elem1 + elem2);
                //stack_dump(stack, "to_check", "ADD");
                break;
            }
            case SUB:
            {
                stack_elem_t elem1 = 0, elem2 = 0;
                STACK_POP(stack, &elem1);
                STACK_POP(stack, &elem2);
                STACK_PUSH(stack, elem2 - elem1);
                //stack_dump(stack, "to_check", "SUB");
                break;
            }
            case DIV:
            {
                stack_elem_t elem1 = 0, elem2 = 0;
                STACK_POP(stack, &elem1);
                STACK_POP(stack, &elem2);
                STACK_PUSH(stack, elem2 / elem1);
                //stack_dump(stack, "to_check", "DIV");
                break;
            }
            case OUT:
            {
                STACK_POP(stack, &res);
                //stack_dump(stack, "to_check", "OUT");
                break;
            }
            case HLT:
            {
                STACK_DTOR(stack, SUCCESSFUL_RETURN);
                return res;
            }
            case UC:
                return POISON;
        }
    }

    return POISON;
}
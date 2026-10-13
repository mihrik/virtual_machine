#include "vm.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

error_codes args_analysis(int argc, const char * const argv[], FILE **fp)
{
    assert(argv);
    assert(fp);

    if (argc == 2)
    {
        *fp = fopen(argv[1], "rb");
        return SUCCESSFUL_RETURN;
    }
    else
    {
        return INVALID_QUANTITY_OF_ARGUMENTS;
    }
}

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

stack_elem_t eval(vm_t *vm, FILE *fp)
{
    assert(vm);
    assert(fp);

    stack_elem_t res = 0;
    commands command = UC;

    stack_elem_t *code = NULL;
    code = read_text(fp);

    if (code == NULL)
    {
        CHECK_ERROR("CODE_MEMORY_ALLOCATION_ERROR\n");
    }

    do
    {
        command = (commands)code[vm->ip];

        switch(command)
        {
            case PUSH:
            {
                vm->ip++;
                stack_elem_t elem = 0;

                elem = code[vm->ip];

                STACK_PUSH(&(vm->stack), elem);

                //stack_dump(stack, "to_check", "PUSH");
                break;
            }
            case ADD:
            {
                stack_elem_t elem1 = 0, elem2 = 0;

                STACK_POP(&(vm->stack), &elem1);
                STACK_POP(&(vm->stack), &elem2);
                STACK_PUSH(&(vm->stack), elem1 + elem2);

                //stack_dump(stack, "to_check", "ADD");
                break;
            }
            case SUB:
            {
                stack_elem_t elem1 = 0, elem2 = 0;

                STACK_POP(&(vm->stack), &elem1);
                STACK_POP(&(vm->stack), &elem2);
                STACK_PUSH(&(vm->stack), elem2 - elem1);
                //stack_dump(stack, "to_check", "SUB");
                break;
            }
            case DIV:
            {
                stack_elem_t elem1 = 0, elem2 = 0;

                STACK_POP(&(vm->stack), &elem1);
                STACK_POP(&(vm->stack), &elem2);
                STACK_PUSH(&(vm->stack), elem2 / elem1);

                //stack_dump(stack, "to_check", "DIV");
                break;
            }
            case MUL:
            {
                stack_elem_t elem1 = 0, elem2 = 0;

                STACK_POP(&(vm->stack), &elem1);
                STACK_POP(&(vm->stack), &elem2);
                STACK_PUSH(&(vm->stack), elem2 * elem1);

                //stack_dump(stack, "to_check", "DIV");
                break;
            }
            case POPr:
            {
                vm->ip++;
                registers reg = (registers)code[vm->ip];
                stack_elem_t elem = 0;
                STACK_POP(&(vm->stack), &elem);

                vm->registers[reg] = elem;
                break;
            }
            case PUSHr:
            {
                vm->ip++;
                registers reg = (registers)code[vm->ip];
                STACK_PUSH(&(vm->stack), vm->registers[reg]);

                break;
            }
            case JMP:
            {
                vm->ip = (size_t)code[vm->ip + 1] - 1;

                break;
            }
            case OUT:
            {
                STACK_POP(&(vm->stack), &res);
                printf("now res s equal to <" deb_spec ">\n", res);
                getchar();

                //stack_dump(stack, "to_check", "OUT");
                break;
            }
            case HLT:
            {
                STACK_DTOR(&(vm->stack), SUCCESSFUL_RETURN);
                free(code);

                return res;
            }
            case UC:
                free(code);

                return POISON;
            default:
                exit(0);
        }
        vm->ip++;
    } while (command != HLT);

    free(code);
    return POISON;
}

stack_elem_t * read_text(FILE *fp)
{
    assert(fp);

    long buf_size = get_file_size(fp) / (long)sizeof(stack_elem_t);
    stack_elem_t *buffer = (stack_elem_t *)calloc((size_t)buf_size, sizeof(stack_elem_t));
    if (buffer == NULL)
        return NULL;


    fread(buffer, sizeof(stack_elem_t), (size_t)buf_size, fp);

    return buffer;
}

long get_file_size(FILE *fp)
{
    assert(fp);

    int file_num =fileno(fp);
    struct stat file_info = {};
    fstat(file_num, &file_info);

    return file_info.st_size;
}

registers get_register(const char *reg)
{
    REGISTER_CMP(reg, AX);
    REGISTER_CMP(reg, BX);
    REGISTER_CMP(reg, CX);
    REGISTER_CMP(reg, DX);

    return UR;
}
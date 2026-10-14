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
                vm_push(vm, code);

                break;
            }
            case ADD:
            {
                vm_add(vm);

                break;
            }
            case SUB:
            {
                vm_sub(vm);

                break;
            }
            case DIV:
            {
                vm_div(vm);

                break;
            }
            case MUL:
            {
                vm_mul(vm);

                break;
            }
            case POPr:
            {
                vm_popr(vm, code);

                break;
            }
            case PUSHr:
            {
                vm_pushr(vm, code);

                break;
            }
            case JMP:
            {
                vm_jmp(vm, code);

                break;
            }
            case JA:
            {
                vm_ja(vm, code);

                break;
            }
            case JAE:
            {
                vm_jae(vm, code);

                break;
            }
            case JB:
            {
                vm_jb(vm, code);

                break;
            }
            case JBE:
            {
                vm_jbe(vm, code);

                break;
            }
            case JE:
            {
                vm_je(vm, code);

                break;
            }
            case JNE:
            {
                vm_jne(vm, code);

                break;
            }
            case IN:
            {
                vm_in(vm);

                break;
            }
            case CALL:
            {
                vm_call(vm, code);

                break;
            }
            case RET:
            {
                vm_ret(vm);

                break;
            }
            case OUT:
            {
                vm_out(vm, &res);

                break;
            }
            case HLT:
            {
                vm_hlt(vm, code);

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

error_codes vm_push(vm_t *vm, stack_elem_t *code)
{
    vm->ip++;
    stack_elem_t elem = 0;

    elem = code[vm->ip];

    STACK_PUSH(&(vm->stack), elem);
    return SUCCESSFUL_RETURN;
}

error_codes vm_add(vm_t *vm)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    STACK_PUSH(&(vm->stack), elem1 + elem2);

    return SUCCESSFUL_RETURN;
}

error_codes vm_sub(vm_t *vm)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    STACK_PUSH(&(vm->stack), elem2 - elem1);

    return SUCCESSFUL_RETURN;
}

error_codes vm_div(vm_t *vm)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    STACK_PUSH(&(vm->stack), elem2 / elem1);

    return SUCCESSFUL_RETURN;
}

error_codes vm_mul(vm_t *vm)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    STACK_PUSH(&(vm->stack), elem2 * elem1);

    return SUCCESSFUL_RETURN;
}

error_codes vm_popr(vm_t *vm, stack_elem_t *code)
{
    vm->ip++;
    registers reg = (registers)code[vm->ip];
    stack_elem_t elem = 0;
    STACK_POP(&(vm->stack), &elem);

    vm->registers[reg] = elem;

    return SUCCESSFUL_RETURN;
}

error_codes vm_pushr(vm_t *vm, stack_elem_t *code)
{
    vm->ip++;
    registers reg = (registers)code[vm->ip];
    STACK_PUSH(&(vm->stack), vm->registers[reg]);

    return SUCCESSFUL_RETURN;
}

error_codes vm_jmp(vm_t *vm, stack_elem_t *code)
{
    vm->ip = (size_t)code[vm->ip + 1] - 1;
    return SUCCESSFUL_RETURN;
}

error_codes vm_out(vm_t *vm, stack_elem_t *res)
{
    STACK_POP(&(vm->stack), res);
    printf("now res s equal to <" deb_spec ">\n", *res);

    return SUCCESSFUL_RETURN;
}

error_codes vm_hlt(vm_t *vm, stack_elem_t *code)
{
    STACK_DTOR(&(vm->stack), SUCCESSFUL_RETURN);
    STACK_DTOR(&(vm->funcs), SUCCESSFUL_RETURN);
    free(code);

    return SUCCESSFUL_RETURN;
}

error_codes vm_ja(vm_t *vm, stack_elem_t *code)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    if (elem2 > elem1)
        vm_jmp(vm, code);
    else
        vm->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes vm_jae(vm_t *vm, stack_elem_t *code)
{
    stack_elem_t elem1 = 0, elem2 = 0;
    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    if (elem2 >= elem1)
        vm_jmp(vm, code);
    else
        vm->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes vm_jb(vm_t *vm, stack_elem_t *code)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    if (elem2 < elem1)
        vm_jmp(vm, code);
    else
        vm->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes vm_jbe(vm_t *vm, stack_elem_t *code)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    if (elem2 <= elem1)
        vm_jmp(vm, code);
    else
        vm->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes vm_je(vm_t *vm, stack_elem_t *code)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    if (elem2 == elem1)
        vm_jmp(vm, code);
    else
        vm->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes vm_jne(vm_t *vm, stack_elem_t *code)
{
    stack_elem_t elem1 = 0, elem2 = 0;

    STACK_POP(&(vm->stack), &elem1);
    STACK_POP(&(vm->stack), &elem2);
    if (elem2 != elem1)
        vm_jmp(vm, code);
    else
        vm->ip += 1;

    return SUCCESSFUL_RETURN;
}

error_codes vm_in(vm_t *vm)
{
    stack_elem_t elem = 0;
    scanf(deb_spec, &elem);

    STACK_PUSH(&(vm->stack), elem);

    return SUCCESSFUL_RETURN;
}

error_codes vm_call(vm_t *vm, stack_elem_t *code)
{
    STACK_PUSH(&(vm->funcs), (stack_elem_t)vm->ip + 1);
    vm_jmp(vm, code);

    return SUCCESSFUL_RETURN;
}

error_codes vm_ret(vm_t *vm)
{
    stack_elem_t new_ip = 0;

    STACK_POP(&(vm->funcs), &new_ip);

    vm->ip = (size_t)new_ip;

    return SUCCESSFUL_RETURN;
}
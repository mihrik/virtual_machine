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

#define VM_JMP_CONDITIONAL_(vm, code, sign) {\
    stack_elem_t elem1 = 0, elem2 = 0;\
    \
    STACK_POP(&((vm)->stack), &elem1);\
    STACK_POP(&((vm)->stack), &elem2);\
    if (elem2 sign elem1)\
        vm_jmp((vm), (code));\
    else\
        (vm)->ip += 1;\
}

#define VM_ARITHMETIC(vm, sign) {\
    stack_elem_t elem1 = 0, elem2 = 0;\
    \
    STACK_POP(&(vm->stack), &elem1);\
    STACK_POP(&(vm->stack), &elem2);\
    STACK_PUSH(&(vm->stack), elem2 sign elem1);\
}

stack_elem_t eval(vm_t *vm, FILE *fp)
{
    assert(vm);
    assert(fp);

    stack_elem_t res = 0;
    commands command = CMD_UC;

    do
    {
        command = (commands)(vm->code)[vm->ip];

        switch(command)
        {
            case CMD_PUSH:
            {
                vm_push(vm, vm->code);

                break;
            }
            case CMD_ADD:
            {
                VM_ARITHMETIC(vm, +);

                break;
            }
            case CMD_SUB:
            {
                VM_ARITHMETIC(vm, -);

                break;
            }
            case CMD_DIV:
            {
                VM_ARITHMETIC(vm, /);

                break;
            }
            case CMD_MUL:
            {
                VM_ARITHMETIC(vm, *);

                break;
            }
            case CMD_POPr:
            {
                vm_popr(vm, vm->code);

                break;
            }
            case CMD_PUSHr:
            {
                vm_pushr(vm, vm->code);

                break;
            }
            case CMD_JMP:
            {
                vm_jmp(vm, vm->code);

                break;
            }
            case CMD_JA:
            {
                VM_JMP_CONDITIONAL_(vm, vm->code, >);

                break;
            }
            case CMD_JAE:
            {
                VM_JMP_CONDITIONAL_(vm, vm->code, >=);

                break;
            }
            case CMD_JB:
            {
                VM_JMP_CONDITIONAL_(vm, vm->code, <);

                break;
            }
            case CMD_JBE:
            {
                VM_JMP_CONDITIONAL_(vm, vm->code, <=);

                break;
            }
            case CMD_JE:
            {
                VM_JMP_CONDITIONAL_(vm, vm->code, ==);

                break;
            }
            case CMD_JNE:
            {
                VM_JMP_CONDITIONAL_(vm, vm->code, !=);

                break;
            }
            case CMD_IN:
            {
                vm_in(vm);

                break;
            }
            case CMD_CALL:
            {
                vm_call(vm, vm->code);

                break;
            }
            case CMD_RET:
            {
                vm_ret(vm);

                break;
            }
            case CMD_SQRT:
            {
                vm_sqrt(vm);

                break;
            }
            case CMD_OUT:
            {
                vm_out(vm, &res);

                break;
            }
            case CMD_HLT:
            {
                vm_hlt(vm, vm->code);

                return res;
            }
            case CMD_UC:
                free(vm->code);

                return POISON;
            default:
                exit(0);
        }
        vm->ip++;
    } while (command != CMD_HLT);

    free(vm->code); // TODO hlt huinya
    return POISON;
}

#undef VM_ARITHMETIC
#undef VM_JMP_CONDITIONAL

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

error_codes vm_sqrt(vm_t *vm)
{
    stack_elem_t elem = 0;

    STACK_POP(&(vm->stack), &elem);
    elem = (stack_elem_t)sqrt(elem);
    STACK_PUSH(&(vm->stack), elem);

    return SUCCESSFUL_RETURN;
}

error_codes vm_ctor(vm_t *vm, FILE *fp)
{
    STACK_CTOR(&(vm->stack), 10, "stack", __FILE__, __func__, __LINE__);
    STACK_CTOR(&(vm->funcs), 10, "funcs", __FILE__, __func__, __LINE__);

    vm->code = read_text(fp);

    if (vm->code == NULL)
        return MEMORY_ALLOCATION_ERROR;

    return SUCCESSFUL_RETURN;
}

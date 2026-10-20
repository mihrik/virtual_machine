#include "vm.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

error_codes (*to_execute[])(vm_t *vm) =
{
    vm_uc,
    vm_push,
    vm_add,
    vm_sub,
    vm_div,
    vm_out,
    vm_popr,
    vm_mul,
    vm_jmp,
    vm_ja,
    vm_jae,
    vm_jb,
    vm_jbe,
    vm_je,
    vm_jne,
    vm_in,
    vm_call,
    vm_ret,
    vm_sqrt,
    vm_popm,
    vm_draw,
    vm_popmr,
    vm_hlt
};

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

error_codes eval(vm_t *vm, FILE *fp)
{
    assert(vm);
    assert(fp);

    commands command = CMD_UC;

    do
    {
        assert(vm->ip < vm->code_size);
        command = (commands)(vm->code)[vm->ip];

        to_execute[(command) & 0x1F](vm);

        vm->ip++;
    } while ((command & 0x1F) != CMD_HLT);

    return UNKNOWN_COMMAND;
}

stack_elem_t * read_text(FILE *fp, vm_t *vm)
{
    assert(fp);

    long buf_size = get_file_size(fp) / (long)sizeof(stack_elem_t);
    vm->code_size = (size_t) buf_size;
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

error_codes vm_push(vm_t *vm)
{
    stack_elem_t res = 0;
    stack_elem_t *args = vm->code + vm->ip + 1;

    if (vm->code[vm->ip] & 0x20)
    {
        res += *args;
        args++;
    }
    if (vm->code[vm->ip] & 0x40)
    {
        res += vm->registers[*args];
        args++;
    }
    if (vm->code[vm->ip] & 0x80)
    {
        res = vm->RAM[res];
    }

    vm->ip = (size_t)(args - vm->code) - 1;

    STACK_PUSH(&(vm->stack), res);

    return SUCCESSFUL_RETURN;
}

error_codes vm_popr(vm_t *vm)
{
    vm->ip++;
    registers reg = (registers)(vm->code)[vm->ip];
    stack_elem_t elem = 0;
    STACK_POP(&(vm->stack), &elem);

    vm->registers[reg] = elem;

    return SUCCESSFUL_RETURN;
}

error_codes vm_jmp(vm_t *vm)
{
    vm->ip = (size_t)(vm->code)[vm->ip + 1] - 1;
    return SUCCESSFUL_RETURN;
}

error_codes vm_out(vm_t *vm)
{
    stack_elem_t res = 0;

    STACK_POP(&(vm->stack), &res);
    printf("now res s equal to <" deb_spec ">\n", res);

    return SUCCESSFUL_RETURN;
}

error_codes vm_hlt(vm_t *vm)
{
    STACK_DTOR(&(vm->stack), SUCCESSFUL_RETURN);
    STACK_DTOR(&(vm->funcs), SUCCESSFUL_RETURN);
    free_vm(vm);

    return SUCCESSFUL_RETURN;
}

error_codes vm_in(vm_t *vm)
{
    stack_elem_t elem = 0;
    scanf(deb_spec, &elem);

    STACK_PUSH(&(vm->stack), elem);

    return SUCCESSFUL_RETURN;
}

error_codes vm_call(vm_t *vm)
{
    STACK_PUSH(&(vm->funcs), (stack_elem_t)vm->ip + 1);
    vm_jmp(vm);

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

    vm->code = read_text(fp, vm);
    vm->RAM = (stack_elem_t *) calloc(RAM_SIZE, sizeof(stack_elem_t));

    if (vm->code == NULL)
        return MEMORY_ALLOCATION_ERROR;

    return SUCCESSFUL_RETURN;
}

error_codes vm_popm(vm_t *vm)
{
    vm->ip++;
    stack_elem_t elem = 0;

    STACK_POP(&(vm->stack), &elem);

    (vm->RAM)[(vm->code)[vm->ip]] = elem;

    return SUCCESSFUL_RETURN;
}

error_codes vm_draw(vm_t *vm)
{
    size_t count = 0;
    while (vm->RAM[count] != 0)
    {
        count++;
    }

    size_t x = (size_t) sqrt(2 * count);
    size_t y = x / 2 + 1;

    for (size_t i = 0; i < y; i++)
    {
        for (size_t j = 0; j < x; j++)
            putchar(vm->RAM[i * x + j]);
        putchar('\n');
    }

    return SUCCESSFUL_RETURN;
}

error_codes vm_popmr(vm_t *vm)
{
    vm->ip++;

    stack_elem_t elem = 0;
    STACK_POP(&(vm->stack), &elem);

    (vm->RAM)[vm->registers[vm->code[vm->ip]]] = elem;

    return SUCCESSFUL_RETURN;
}

void vm_dump(vm_t *vm, const char *reason, const char *process)
{
    ONDBG(
        PRINT_COLOR(EXTRA_RED, "=== STACK ===\n");
        stack_dump(&(vm->stack), reason, process);
        putchar('\n');

        PRINT_COLOR(EXTRA_RED, "=== FUNCS ===\n");
        stack_dump(&(vm->funcs), reason, process);
        putchar('\n');

        PRINT_COLOR(EXTRA_RED, "=== BIN_CODE ===\n");
        for (size_t i = 0; i < vm->code_size / 4; i++)
        {
            PRINT_COLOR(YELLOW, "[%p]:", vm->code + 4 * i);

            for (size_t k = 0; k < 4; k++)
            {
                if (i * 4 + k == vm->ip)
                {
                    PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[i * 4 + k] >> 4 & 0xF, (unsigned)vm->code[i * 4 + k] & 0xF);
                    PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[i * 4 + k] >> 12 & 0xF, (unsigned)vm->code[i * 4 + k] >> 8 & 0xF);
                    PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[i * 4 + k] >> 20 & 0xF, (unsigned)vm->code[i * 4 + k] >> 16 & 0xF);
                    PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[i * 4 + k] >> 28 & 0xF, (unsigned)vm->code[i * 4 + k] >> 24 & 0xF);
                }
                else if (k % 2 == 0)
                {
                    PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[i * 4 + k] >> 4 & 0xF, (unsigned)vm->code[i * 4 + k] & 0xF);
                    PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[i * 4 + k] >> 12 & 0xF, (unsigned)vm->code[i * 4 + k] >> 8 & 0xF);
                    PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[i * 4 + k] >> 20 & 0xF, (unsigned)vm->code[i * 4 + k] >> 16 & 0xF);
                    PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[i * 4 + k] >> 28 & 0xF, (unsigned)vm->code[i * 4 + k] >> 24 & 0xF);
                }
                else
                {
                    PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[i * 4 + k] >> 4 & 0xF, (unsigned)vm->code[i * 4 + k] & 0xF);
                    PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[i * 4 + k] >> 12 & 0xF, (unsigned)vm->code[i * 4 + k] >> 8 & 0xF);
                    PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[i * 4 + k] >> 20 & 0xF, (unsigned)vm->code[i * 4 + k] >> 16 & 0xF);
                    PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[i * 4 + k] >> 28 & 0xF, (unsigned)vm->code[i * 4 + k] >> 24 & 0xF);
                }
            }
            putchar('\n');
        }
        if (vm->code_size > vm->code_size / 4 * 4)
            PRINT_COLOR(YELLOW, "[%p]:", vm->code + vm->code_size / 4 * 4);
        for (size_t k = 0; k + vm->code_size / 4 * 4 < vm->code_size; k++)
        {
            if (vm->code_size / 4 * 4 + k == vm->ip)
            {
                PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 4 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] & 0xF);
                PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 12 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 8 & 0xF);
                PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 20 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 16 & 0xF);
                PRINT_COLOR(YELLOW, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 28 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 24 & 0xF);
            }
            else if (k % 2 == 0)
            {
                PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 4 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] & 0xF);
                PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 12 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 8 & 0xF);
                PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 20 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 16 & 0xF);
                PRINT_COLOR(BLUE, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 28 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 24 & 0xF);
            }
            else
            {
                PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 4 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] & 0xF);
                PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 12 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 8 & 0xF);
                PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 20 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 16 & 0xF);
                PRINT_COLOR(RED, " %X%X", (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 28 & 0xF, (unsigned)vm->code[vm->code_size / 4 * 4 + k] >> 24 & 0xF);
            }
        }
        putchar('\n');
        putchar('\n');

        PRINT_COLOR(EXTRA_RED, "=== REGISTERS ===\n");
        for (size_t i = 1; i < sizeof(vm->registers) / sizeof(stack_elem_t); i++)
        {
            PRINT_COLOR(RED, "%c%c:", (char)(64 + i), 'X');
            PRINT_COLOR(BLUE, " " deb_spec "\n", vm->registers[i]);
        }
        putchar('\n');
    )
}

error_codes free_vm(vm_t *vm)
{
    free(vm->RAM);
    free(vm->code);

    return SUCCESSFUL_RETURN;
}

error_codes vm_add(vm_t *vm)
{
    VM_ARITHMETIC(vm, +);

    return SUCCESSFUL_RETURN;
}

error_codes vm_sub(vm_t *vm)
{
    VM_ARITHMETIC(vm, -);

    return SUCCESSFUL_RETURN;
}

error_codes vm_div(vm_t *vm)
{
    VM_ARITHMETIC(vm, /);

    return SUCCESSFUL_RETURN;
}

error_codes vm_mul(vm_t *vm)
{
    VM_ARITHMETIC(vm, *);

    return SUCCESSFUL_RETURN;
}

error_codes vm_uc(vm_t *vm)
{
    return UNKNOWN_COMMAND;
}

error_codes vm_ja(vm_t *vm)
{
    VM_JMP_CONDITIONAL(vm, >);

    return SUCCESSFUL_RETURN;
}

error_codes vm_jae(vm_t *vm)
{
    VM_JMP_CONDITIONAL(vm, >=);

    return SUCCESSFUL_RETURN;
}

error_codes vm_jb(vm_t *vm)
{
    VM_JMP_CONDITIONAL(vm, <);

    return SUCCESSFUL_RETURN;
}

error_codes vm_jbe(vm_t *vm)
{
    VM_JMP_CONDITIONAL(vm, <=);

    return SUCCESSFUL_RETURN;
}

error_codes vm_je(vm_t *vm)
{
    VM_JMP_CONDITIONAL(vm, ==);

    return SUCCESSFUL_RETURN;
}

error_codes vm_jne(vm_t *vm)
{
    VM_JMP_CONDITIONAL(vm, !=);

    return SUCCESSFUL_RETURN;
}
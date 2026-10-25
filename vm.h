#ifndef VM_H
#define VM_H
#include <string.h>
#include "enum.h"

const int RAM_SIZE = 6000;
const int DISPLAY_WIDTH = 100;
const int DISPLAY_HEIGHT = 50;

typedef struct buf_data
{
    size_t char_num;
    size_t strings_num;
} buf_data;

typedef struct vm_t
{
    stack_t stack;
    stack_t funcs;
    stack_elem_t *code;
    stack_elem_t *RAM;
    size_t ip;
    size_t code_size;
    stack_elem_t registers[10];
} vm_t;

#define VM_JMP_CONDITIONAL(vm, sign) {\
    stack_elem_t elem1 = 0, elem2 = 0;\
    \
    STACK_POP(&((vm)->stack), &elem1);\
    STACK_POP(&((vm)->stack), &elem2);\
    if (elem2 sign elem1)\
        vm_jmp((vm));\
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

error_codes eval(vm_t *stack, FILE *fp);
stack_elem_t * read_text(FILE *fp, vm_t *vm);
long get_file_size(FILE *fp);
error_codes vm_push(vm_t *vm);
error_codes vm_popr(vm_t *vm);
error_codes vm_jmp(vm_t *vm);
error_codes vm_out(vm_t *vm);
error_codes vm_hlt(vm_t *vm);
error_codes vm_in(vm_t *vm);
error_codes vm_call(vm_t *vm);
error_codes vm_ret(vm_t *vm);
error_codes vm_ctor(vm_t *vm, FILE *fp);
error_codes vm_sqrt(vm_t *vm);
error_codes vm_popm(vm_t *vm);
error_codes vm_draw(vm_t *vm);
error_codes vm_popmr(vm_t *vm);
void vm_dump(vm_t *vm, const char *reason, const char *process);
error_codes free_vm(vm_t *vm);
error_codes vm_add(vm_t *vm);
error_codes vm_sub(vm_t *vm);
error_codes vm_mul(vm_t *vm);
error_codes vm_div(vm_t *vm);
error_codes vm_uc(vm_t *vm);
error_codes vm_ja(vm_t *vm);
error_codes vm_jae(vm_t *vm);
error_codes vm_jb(vm_t *vm);
error_codes vm_jbe(vm_t *vm);
error_codes vm_je(vm_t *vm);
error_codes vm_jne(vm_t *vm);

#endif
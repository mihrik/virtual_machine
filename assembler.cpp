#include "vm.h"
#include <string.h>

int main(int argc, const char * const argv[])
{
    FILE *fp_r = NULL;
    FILE *fp_w = fopen("compiled.bin", "w");

    args_analysis(argc, argv, &fp_r);


    asm_t assembler = {.line = {}, .command_line = {}, .buffer = NULL, .command = CMD_UC, .ip = 0, .labels = {}};
    asm_ctor(&assembler);

    assemble(fp_r, &assembler);

    fwrite(assembler.buffer, sizeof(stack_elem_t), assembler.ip, fp_w);
    free(assembler.buffer);

    return 0;
}
#include "assembler.h"
#include <string.h>

int main(int argc, const char * const argv[])
{
    FILE *fp_r = NULL;
    FILE *fp_w = fopen("compiled.bin", "w");
    error_codes err = SUCCESSFUL_RETURN;

    if ((err = args_analysis(argc, argv, &fp_r)) != SUCCESSFUL_RETURN)
    {
        PRINT_COLOR(EXTRA_RED, "INVALID_QUANTITY_OF_ARGUMENTS");
        return err;
    }


    asm_t assembler = {.line = {}, .command_line = {}, .buffer = NULL, .command = CMD_UC, .ip = 0, .labels = {}};
    if ((err = asm_ctor(&assembler)) != SUCCESSFUL_RETURN)
    {
        PRINT_COLOR(EXTRA_RED, "MEMORY ALLOCATION ERROR WHILE DOING STACK_CTOR");
        return 0;
    }

    assemble(fp_r, &assembler);

    fwrite(assembler.buffer, sizeof(stack_elem_t), assembler.ip, fp_w);
    free(assembler.buffer);

    return 0;
}
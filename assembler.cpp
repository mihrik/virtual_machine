#include "vm.h"
#include <string.h>

int main(int argc, const char * const argv[])
{
    FILE *fp_r = NULL;
    FILE *fp_w = fopen("compiled.bin", "w");

    args_analysis(argc, argv, &fp_r);

    stack_elem_t *code = NULL;
    size_t ip = 0;

    assemble(fp_r, &code, &ip);

    fwrite(code, sizeof(stack_elem_t), ip, fp_w);
    free(code);

    return 0;
}
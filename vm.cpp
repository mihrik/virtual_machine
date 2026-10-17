#include "vm.h"

int main(int argc, const char * const argv[])
{
    FILE *fp = NULL;
    error_codes err = SUCCESSFUL_RETURN;
    if ((err = args_analysis(argc, argv, &fp)) != SUCCESSFUL_RETURN)
    {
        PRINT_COLOR(EXTRA_RED, "INVALID_QUANTITY_OF_ARGUMENTS");
        return err;
    }

    vm_t vm = {.stack = {}, .funcs = {}, .code = NULL, .RAM = NULL, .ip = 0, .code_size = 0, .registers = {}};
    vm_ctor(&vm, fp);

    stack_elem_t res = eval(&vm, fp);

    fclose(fp);

    return 0;
}
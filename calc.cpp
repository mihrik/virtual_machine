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

    stack_t stack = {};
    STACK_CTOR(&stack, 10, "stack", __FILE__, __func__, __LINE__);

    stack_elem_t res = eval(&stack, fp);

    printf("result is equal to " deb_spec "\n", res);

    fclose(fp);

    return 0;
}
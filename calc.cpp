#include "vm.h"

int main(int argc, const char * const argv[])
{
    FILE *fp = NULL;
    args_analysis(argc, argv, &fp);

    stack_t stack = {};
    STACK_CTOR(&stack, 10, "stack", __FILE__, __func__, __LINE__);

    stack_elem_t res = eval(&stack, fp);

    printf("result is equal to " deb_spec "\n", res);

    fclose(fp);

    return 0;
}
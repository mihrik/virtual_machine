#include "vm.h"

int main(int argc, const char * const argv[])
{
    FILE *fp = NULL;

    args_analysis(argc, argv, &fp);

    char line[MAX_COMMAND_SIZE] = {};
    char command_line[MAX_COMMAND_SIZE] = {};
    commands command = UC;

    while (fgets(line, MAX_COMMAND_SIZE, fp))
    {
        sscanf(line, "%s", command_line);
        command = get_command(command_line);
        printf("%d ", command);

        if (command == PUSH)
        {
            stack_elem_t elem = 0;
            sscanf(line, "%*s" deb_spec, &elem);
            printf(deb_spec, elem);
        }

        putchar('\n');
    }

    return 0;
}
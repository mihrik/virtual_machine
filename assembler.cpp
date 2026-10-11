#include "vm.h"

int main(int argc, const char * const argv[])
{
    FILE *fp_r = NULL;

    args_analysis(argc, argv, &fp_r);

    char line[MAX_COMMAND_SIZE] = {};
    char command_line[MAX_COMMAND_SIZE] = {};
    stack_elem_t buffer[MAX_BUFFER_SIZE] = {};
    size_t ip = 0;
    commands command = UC;

    FILE *fp_w = fopen("compiled.bin", "w");

    while (fgets(line, MAX_COMMAND_SIZE, fp_r))
    {
        if (sscanf(line, "%s", command_line) != 1)
        {
            printf("ABSENCE OF THE COMMAND NUMBER" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
            return INVALID_QUANTITY_OF_ARGUMENTS;
        }

        command = get_command(command_line);
        if (command == UC)
        {
            printf("GOT_UNKNOWN_COMMAND: %s, WHILE DOING: %s:%d in function: %s\n", command_line, __FILE__, __LINE__, __func__);
            return UNKNOWN_COMMAND;
        }

        buffer[ip++] = command;

        if (command == PUSH)
        {
            stack_elem_t elem = 0;

            if (sscanf(line, "%*s" deb_spec, &elem) != 1)
            {
                printf("ABSENCE OF THE SECOND PARAMETER OF PUSH" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
                return INVALID_QUANTITY_OF_ARGUMENTS;
            }

            buffer[ip++] = elem;
        }
    }

    fwrite(buffer, sizeof(stack_elem_t), ip, fp_w);

    return 0;
}
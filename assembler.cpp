#include "vm.h"
#include <string.h>

int main(int argc, const char * const argv[])
{
    FILE *fp_r = NULL;

    args_analysis(argc, argv, &fp_r);

    char line[MAX_COMMAND_SIZE] = {};
    char command_line[MAX_COMMAND_SIZE] = {};
    stack_elem_t buffer[MAX_BUFFER_SIZE] = {};
    size_t ip = 0;
    commands command = UC;
    int labels[5] = {-1, -1, -1, -1, -1};

    FILE *fp_w = fopen("compiled.bin", "w");

    while (fgets(line, MAX_COMMAND_SIZE, fp_r))
    {
        if (sscanf(line, "%s", command_line) != 1)
        {
            printf("ABSENCE OF THE COMMAND NUMBER" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
            return INVALID_QUANTITY_OF_ARGUMENTS;
        }

        char *semicolon_ptr = NULL;
        if ((semicolon_ptr = strchr(line, ';')) != NULL)
            *semicolon_ptr = '\0';

        if (line[0] == ':')
        {
            int mark = 0;
            sscanf(line, ":%d", &mark);
            labels[mark] = (int)ip;
            continue;
        }

        command = get_command(command_line);
        if (command == UC)
        {
            printf("GOT_UNKNOWN_COMMAND: %s, WHILE DOING: %s:%d in function: %s\n", command_line, __FILE__, __LINE__, __func__);
            return UNKNOWN_COMMAND;
        }

        buffer[ip++] = command;

        switch(command)
        {
            case PUSH :
                        {
                            stack_elem_t elem = 0;

                            if (sscanf(line, "%*s" deb_spec, &elem) != 1)
                            {
                                printf("ABSENCE OF THE SECOND PARAMETER OF PUSH" " WHILE DOING %s:%d in function: %s\n", __FILE__, __LINE__, __func__);
                                return INVALID_QUANTITY_OF_ARGUMENTS;
                            }

                            buffer[ip++] = elem;

                            break;
                        }
            case PUSHr:
            case POPr :
                        {
                            char reg[MAX_COMMAND_SIZE] = {};
                            sscanf(line, "%*s %s", reg);

                            stack_elem_t rnum = get_register(reg);
                            buffer[ip++] = rnum;

                            break;
                        }
            case JMP  : {
                            size_t mark = 0;

                            sscanf(line, "%*s %zu", &mark);
                            buffer[ip++] = labels[mark];

                            break;
                        }
            default   : break;

        }
    }

    fwrite(buffer, sizeof(stack_elem_t), ip, fp_w);

    return 0;
}
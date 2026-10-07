#include "vm.h"

int main(int argc, const char * const argv[])
{
    FILE *fp = NULL;

    args_analysis(argc, argv, &fp);

    char line[MAX_COMMAND_SIZE] = {};
    int tmp = 0;
    commands command = UC;

    while (fgets(line, MAX_COMMAND_SIZE, fp))
    {
        sscanf(line, "%d", &tmp);
        command = (commands) tmp;

        switch(command)
        {
            case PUSH: printf("PUSH "); break;
            case ADD: printf("ADD "); break;
            case SUB: printf("SUB "); break;
            case OUT: printf("OUT "); break;
            case DIV: printf("DIV "); break;
            case HLT: printf("HLT "); break;
            case UC: printf("UC "); break;
        }

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
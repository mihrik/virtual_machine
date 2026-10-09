#include "vm.h"

int main(int argc, const char * const argv[])
{
    FILE *fp = NULL;

    args_analysis(argc, argv, &fp);

    size_t len = 0;
    commands command = UC;
    int tmp = 0;

    char *text = NULL;
    text = read_text(fp);

    char **lines = NULL;
    lines = getlines(text, &len);

    for (size_t i = 0; i < len; i++)
    {
        sscanf(lines[i], "%d", &tmp);
        command = (commands)tmp;

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
            sscanf(lines[i], "%*d%*c" deb_spec, &elem);
            printf(deb_spec, elem);
        }

        putchar('\n');
    }
    free(text);
    text = NULL;
    free(lines);
    lines = NULL;

    return 0;
}
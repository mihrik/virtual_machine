#include "vm.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

error_codes args_analysis(int argc, const char * const argv[], FILE **fp)
{
    if (argc == 1)
    {
        *fp = stdin;
        return SUCCESSFUL_RETURN;
    }
    else if (argc == 2)
    {
        *fp = fopen(argv[1], "rb");
        return SUCCESSFUL_RETURN;
    }
    else
    {
        return INVALID_QUANTITY_OF_ARGUMENTS;
    }
}

commands get_command(const char *command)
{
    COMMAND_CMP(command, PUSH);
    COMMAND_CMP(command, ADD);
    COMMAND_CMP(command, SUB);
    COMMAND_CMP(command, DIV);
    COMMAND_CMP(command, OUT);
    COMMAND_CMP(command, HLT);

    return UC;
}

stack_elem_t eval(stack_t *stack, FILE *fp)
{
    stack_elem_t res = 0;
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
            case PUSH:
            {
                stack_elem_t elem = 0;
                sscanf(lines[i], "%*d%*c" deb_spec, &elem);
                STACK_PUSH(stack, elem);
                //stack_dump(stack, "to_check", "PUSH");
                break;
            }
            case ADD:
            {
                stack_elem_t elem1 = 0, elem2 = 0;
                STACK_POP(stack, &elem1);
                STACK_POP(stack, &elem2);
                STACK_PUSH(stack, elem1 + elem2);
                //stack_dump(stack, "to_check", "ADD");
                break;
            }
            case SUB:
            {
                stack_elem_t elem1 = 0, elem2 = 0;
                STACK_POP(stack, &elem1);
                STACK_POP(stack, &elem2);
                STACK_PUSH(stack, elem2 - elem1);
                //stack_dump(stack, "to_check", "SUB");
                break;
            }
            case DIV:
            {
                stack_elem_t elem1 = 0, elem2 = 0;
                STACK_POP(stack, &elem1);
                STACK_POP(stack, &elem2);
                STACK_PUSH(stack, elem2 / elem1);
                //stack_dump(stack, "to_check", "DIV");
                break;
            }
            case OUT:
            {
                STACK_POP(stack, &res);
                //stack_dump(stack, "to_check", "OUT");
                break;
            }
            case HLT:
            {
                STACK_DTOR(stack, SUCCESSFUL_RETURN);
                free(text);
                free(lines);
                return res;
            }
            case UC:
                free(text);
                free(lines);
                return POISON;
        }
    }

    return POISON;
}

char * read_text(FILE *fp)
{
    assert(fp);

    long buf_size = get_file_size(fp);
    char *buffer = (char *)calloc((size_t)buf_size + 1, sizeof(char));
    if (buffer == NULL)
        return NULL;

    fread(buffer, sizeof(char), (size_t)buf_size, fp);
    buffer[buf_size] = '\0';

    return buffer;
}

char ** getlines(char *text, size_t *len)
{
    assert(text);
    assert(len);

    buf_data text_info = {.char_num = 0, .strings_num = 1};

    parse_string(&text_info, text);

    char **onegin = (char **)calloc(text_info.strings_num, sizeof(char *));
    if (onegin == NULL)
        return NULL;
    onegin[0] = text;

    *len = fill_lines(onegin, text_info, text);
    return onegin;
}

long get_file_size(FILE *fp)
{
    assert(fp);

    int file_num =fileno(fp);
    struct stat file_info = {};
    fstat(file_num, &file_info);

    return file_info.st_size;
}

void parse_string(buf_data *text_info, char *text)
{   assert(text_info);
    assert(text);


    while(text[text_info->char_num])
    {
        if (text[text_info->char_num] == '\n')
        {
            text_info->strings_num++;
            text[text_info->char_num] = '\0';
        }

        text_info->char_num++;
    }
}

size_t fill_lines(char **onegin, buf_data text_info, char *text)
{
    assert(onegin);
    assert(text);

    size_t pointer = 1;
    for (size_t j = 0; j < text_info.char_num; j++)
    {
        if (text[j] == '\0')
        {
            onegin[pointer++] = text + j + 1;
        }
    }

    return pointer;
}
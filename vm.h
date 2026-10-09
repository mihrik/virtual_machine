#include "stack.h"
#include <string.h>

const int MAX_COMMAND_SIZE = 40;


typedef enum commands
{
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    DIV = 4,
    OUT = 5,
    HLT = 0xF4,
    UC= 666
} comands;

typedef struct buf_data
{
    size_t char_num;
    size_t strings_num;
} buf_data;

#define CHECK_ERROR(message) {\
    PRINT_COLOR(EXTRA_RED, #message " WHILE DOING %s:%d in function: %s", __FILE__, __LINE__, __func__);\
    free(text);\
    free(code);\
    return POISON;\
}

#define COMMAND_CMP(to_check, reference) {\
     if (strcmp(to_check, #reference) == 0)\
        return reference;\
}

error_codes args_analysis(int argc, const char * const argv[], FILE **fp);
commands get_command(const char *command);
stack_elem_t eval(stack_t *stack, FILE *fp);
char * read_text(FILE *fp);
long get_file_size(FILE *fp);
void parse_string(buf_data *text_info, char *text);
size_t fill_lines(char **onegin, buf_data text_info, char *text);
char ** getlines(char *text, size_t *len);
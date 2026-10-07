#define RED "\033[91m"
#define EXTRA_RED "\033[31m"
#define GREEN "\033[92m"
#define ORANGE "\033[33m"
#define BLUE "\033[94m"
#define YELLOW "\033[93m"
#define PURPLE "\033[95m"
#define EXTRA_BLUE "\033[34m"
#define LIGHT_BLUE "\033[96m"
#define EXTRA_GREEN "\033[32m"
#define RESET_COLOR "\033[0m"
#define BOLD_BLUE "\033[94;1m"
#define BOLD_RED "\033[91;1m"
#define PRINT_COLOR(x, ...)  {    \
    printf(x __VA_ARGS__);        \
    printf(RESET_COLOR);          \
}

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include "colors.h"
#include <stdbool.h>


typedef int stack_elem_t;
#define deb_spec "%d"
const stack_elem_t POISON = 666;
const stack_elem_t LEFT_CANARY = 0xEDA | 0x40A0B000;
const stack_elem_t RIGHT_CANARY = 0xEDA | 0x40A0B000;
const size_t MAX_CAPACITY = 10000;

#define STACK_ON_DEBUG
#define CANARY_DEBUG_ON
#define HASH_DEBUG_ON

typedef enum error_codes
{
    SUCCESSFUL_RETURN = 0,
    MEMORY_ALLOCATION_ERROR = 10,
    NULL_STACK_MEANING = 11,
    SIZE_MORE_THAN_CAPACITY = 12,
    ZERO_CAPACITY_ERROR = 13,
    POP_FROM_EMPTY = 14,
    NULL_DATA_MEANING = 15,
    NULL_VALUE_RETURN = 16,
    POISON_ELEMENT_MENTION = 17,
    LEFT_CANARY_LOSE = 18,
    RIGHT_CANARY_LOSE = 19,
    LEFT_STACK_CANARY_LOSE = 20,
    RIGHT_STACK_CANARY_LOSE = 21,
    HASH_MEANING_CHANGED = 22,
    ENORMOUS_CAPACITY = 23,
    INVALID_QUANTITY_OF_ARGUMENTS = 24,
    UNKNOWN_COMMAND = 25
} error_codes;

#ifdef STACK_ON_DEBUG
    #define ONDBG(...) __VA_ARGS__
    #define STACK_DTOR(stack, error) {                                                                     \
                stack_dtor((stack), (error));                                                              \
                if (is_okay("STACK_DTOR", (stack), error) == ABORT_PROCESS)                                \
                {                                                                                          \
                    return error;                                                                          \
                }                                                                                          \
            }

    #define STACK_CTOR(stack, capacity, name, file, func, line) {                                          \
                error_codes error = stack_ctor((stack), (capacity), (name), __FILE__, __func__, __LINE__); \
                if (is_okay("STACK.CTOR", (stack), (error)) == ABORT_PROCESS)                              \
                {                                                                                          \
                    stack_dtor((stack), error);                                                            \
                    return error;                                                                          \
                }                                                                                          \
            }

    #define STRINGISATION(line) #line
    #define TO_STRING(line) STRINGISATION(line)

    #define STACK_PUSH(stack, elem) {                                                                     \
                error_codes error = stack_push((stack), (elem));                                          \
                if (is_okay("STACK_PUSH "  __FILE__ ":" TO_STRING(__LINE__), (stack), (error)) == ABORT_PROCESS)\
                {                                                                                         \
                    stack_dtor((stack), error);                                                           \
                    return error;                                                                         \
                }                                                                                         \
            }

    #define STACK_POP(stack, rtrn_val) {                                                                  \
                error_codes error = stack_pop((stack), (rtrn_val));                                       \
                if (is_okay("STACK_POP " __FILE__ ":" TO_STRING(__LINE__), (stack), error) == ABORT_PROCESS)\
                {                                                                                         \
                    stack_dtor((stack), error);                                                           \
                    return error;                                                                         \
                }                                                                                         \
            }

#else
    #define ONDBG(...)
    #define STACK_CTOR(stack, capacity, name, file, func, line) stack_ctor(stack, capacity)
    #define STACK_PUSH(stack, elem) stack_push(stack, elem)
    #define STACK_POP(stack, rtrn_val) stack_pop(stack, rtrn_val)
    #define STACK_DTOR(stack, error) stack_dtor(stack, error)
#endif

#ifdef CANARY_DEBUG_ON
    #define CDO(...) __VA_ARGS__
#else
    #define CDO(...)
#endif

#ifdef HASH_DEBUG_ON
    #define HDO(...) __VA_ARGS__
#else
    #define HDO(...)
#endif

typedef enum conclusion
{
    ABORT_PROCESS = 0,
    CONTINUE_PROCESS = 1
} conclusion;

typedef struct stack_t
{
    CDO( stack_elem_t lcanary  = LEFT_CANARY;)
    ONDBG(const char *name; const char *file; const char *func; size_t line;)
    stack_elem_t *data;
    size_t size;
    size_t capacity;
    size_t hash;
    CDO(const stack_elem_t rcanary = RIGHT_CANARY;)
} stack_t;

error_codes stack_ctor(stack_t *stack, size_t capacity ONDBG(,const char *name, const char *file, const char *func, size_t line));
error_codes stack_push(stack_t *stack, stack_elem_t elem);
error_codes stack_pop(stack_t *stack, stack_elem_t *rtrn_val);
void stack_dtor(stack_t *stack, error_codes error);
void stack_dump(stack_t *stack, const char *reason, const char *process);
error_codes check_errors(stack_t *stack);
conclusion is_okay(const char *process, stack_t *stack, error_codes error);
void poison_stack(size_t begin, size_t end, stack_t *stack);
error_codes check_allocation(stack_elem_t *data, size_t desirable_size);
size_t djb2(stack_elem_t element);
size_t hash_eval(stack_t stack);
void hash_set(stack_t *stack);
error_codes hash_check(stack_t stack);

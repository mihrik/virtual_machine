#include "stack.h"

error_codes stack_ctor(stack_t *stack, size_t capacity ONDBG(,const char *name, const char *file, const char *func, size_t line))
{
    error_codes code = SUCCESSFUL_RETURN;
    if (stack == NULL)
        return NULL_STACK_MEANING;

    if (capacity > MAX_CAPACITY)
        return ENORMOUS_CAPACITY;

    ONDBG
    (
        stack->name = name;
        stack->file = file;
        stack->func = func;
        stack->line = line;
    )
    stack->capacity = capacity;
    stack->size = 0;
    stack->data = (stack_elem_t *) malloc((capacity + 2) * sizeof(stack_elem_t));

    ONDBG(
        code = check_allocation(stack->data, (capacity + 2) * sizeof(stack_elem_t));
    )

    CDO(
    stack->data[0] = LEFT_CANARY;
    stack->data[stack->capacity + 1] = RIGHT_CANARY;
    )

    poison_stack(1, stack->capacity + 1, stack);

    HDO(hash_set(stack);)

    return code;
}

error_codes stack_push(stack_t *stack, stack_elem_t elem)
{
    error_codes code = SUCCESSFUL_RETURN;

    ONDBG(
    if ((code = check_errors(stack)) != SUCCESSFUL_RETURN)
        return code;
    )

    if (stack->size == stack->capacity)
    {
        stack->data = (stack_elem_t *) realloc(stack->data, (stack->capacity * 2  + 2) * sizeof(stack_elem_t));


        ONDBG(
        if (check_allocation(stack->data, (stack->capacity * 2 + 2) * sizeof(stack_elem_t)) != SUCCESSFUL_RETURN)
            return MEMORY_ALLOCATION_ERROR;
        )

        CDO(stack->data[stack->capacity + 1] = RIGHT_CANARY;)
        stack->capacity *= 2;

        poison_stack(stack->size, stack->capacity, stack);
    }

    stack->data[++stack->size] = elem;

    HDO(hash_set(stack);)

    ONDBG(code = check_errors(stack);)

    return code;
}

error_codes stack_pop(stack_t *stack, stack_elem_t *rtrn_val)
{
    error_codes code = SUCCESSFUL_RETURN;

    ONDBG(
    if ((code = check_errors(stack)) != SUCCESSFUL_RETURN)
        return code;

    if (stack->size == 0)
        return POP_FROM_EMPTY;

    if (rtrn_val == NULL)
        return NULL_VALUE_RETURN;
    )

    *rtrn_val = stack->data[stack->size];

    ONDBG(
    if (stack->data[stack->size] == POISON)
        return POISON_ELEMENT_MENTION;
    )

    stack->data[stack->size--] = POISON;

    if (stack->size * 2 == stack->capacity && stack->size != 0 && stack-> size != 1)
    {
        stack->data = (stack_elem_t *) realloc(stack->data, (stack->capacity / 2 + 2) * sizeof(stack_elem_t));

        ONDBG(
        if (check_allocation(stack->data, (stack->capacity / 2 + 2) * sizeof(stack_elem_t)) != SUCCESSFUL_RETURN)
            return MEMORY_ALLOCATION_ERROR;
        )

        CDO(stack->data[stack->capacity + 1] = RIGHT_CANARY;)
        stack->capacity /= 2;
    }

    HDO(hash_set(stack);)

    ONDBG(code = check_errors(stack);)

    return code;
}

void stack_dtor(stack_t *stack, error_codes error)
{
    if (error != NULL_STACK_MEANING)
    {
        if (error != NULL_DATA_MEANING)
            poison_stack(0, stack->capacity, stack);

        free(stack->data);

        stack->data = NULL;
        stack->capacity = 0;
        stack->size = 0;
    }
}

void stack_dump(stack_t *stack, const char *reason, const char *process)
{
    ONDBG(
        printf("\n\n\n");
        PRINT_COLOR(EXTRA_RED, "reason: %s, while doing: %s\n", reason, process);
        printf("stack_t \"%s\"[%p] created by %s at %s:%lu\n", stack->name, stack, stack->func, stack->file, stack->line);
        printf("{\n");
        PRINT_COLOR(BLUE, "    capacity = %lu;\n", stack->capacity);
        PRINT_COLOR(BLUE, "    size = %lu;\n", stack->size);
        PRINT_COLOR(BLUE, "    data[%p]\n", stack->data);
        for (size_t i = 0; i < stack->capacity + 2; i++)
        {
            CDO(
            if (i == 0)
            {
                PRINT_COLOR(PURPLE, "        canary = 0x%lX"  "\n", (size_t)stack->data[i]);
            }
            )
            if (i < stack->size + 1 && i > 0)
            {
                PRINT_COLOR(GREEN, "        *[%lu] = " deb_spec "\n", i - 1, stack->data[i]);
            }
            else if (i >= stack->size + 1 && i != stack->capacity + 1)
            {
                PRINT_COLOR(ORANGE, "         [%lu] = " deb_spec "\n", i - 1, stack->data[i]);
            }
            CDO(
            else if (i == stack->capacity + 1)
            {
                PRINT_COLOR(PURPLE, "        canary = 0x%lX"  "\n", (size_t)stack->data[i]);
            }
            )
        }
        printf("}\n\n\n");
    )
}

error_codes check_errors(stack_t *stack)
{
    if (stack == NULL)
        return NULL_STACK_MEANING;
    CDO(
    if (stack->lcanary != LEFT_CANARY)
        return LEFT_STACK_CANARY_LOSE;

    if (stack->rcanary != RIGHT_CANARY)
        return RIGHT_STACK_CANARY_LOSE;
    )

    if (stack->data == NULL)
        return NULL_DATA_MEANING;

    CDO(
    if (stack->data[0] != LEFT_CANARY)
        return LEFT_CANARY_LOSE;
    )

    if (stack->size > stack->capacity)
        return SIZE_MORE_THAN_CAPACITY;

    if (stack->capacity == 0)
        return ZERO_CAPACITY_ERROR;

    if (stack->capacity > MAX_CAPACITY)
        return ENORMOUS_CAPACITY;

    HDO(
    if (hash_check(*stack) != SUCCESSFUL_RETURN)
        return HASH_MEANING_CHANGED;
    )

    CDO(
    if (stack->data[stack->capacity + 1] != RIGHT_CANARY)
        return RIGHT_CANARY_LOSE;
    )
    return SUCCESSFUL_RETURN;
}

conclusion is_okay(const char *process, stack_t *stack, error_codes error)
{
    bool found_error = false;

    ONDBG(
        switch(error)
        {
            case NULL_STACK_MEANING     : PRINT_COLOR(EXTRA_RED, "reason: NULL_STACK_MEANING, while doing: %s\n", process);
                                          found_error = true;
                                          break;

            case SIZE_MORE_THAN_CAPACITY: stack_dump(stack, "SIZE_MORE_THAN_CAPACITY", process);
                                          found_error = true;
                                          break;

            case ZERO_CAPACITY_ERROR    : stack_dump(stack, "ZERO_CAPACITY_ERROR", process);
                                          found_error = true;
                                          break;

            case MEMORY_ALLOCATION_ERROR: PRINT_COLOR(EXTRA_RED, "reason: MEMORY_ALLOCATION_ERROR, while doing: %s\n", process);
                                          printf("stack_t \"%s\"[%p] created by %s at %s:%lu\n", stack->name, stack, stack->func, stack->file, stack->line);
                                          found_error = true;
                                          break;

            case POP_FROM_EMPTY         : stack_dump(stack, "POP_FROM_EMPTY", process);
                                          found_error = true;
                                          break;

            case NULL_DATA_MEANING      : PRINT_COLOR(EXTRA_RED, "reason: NULL_DATA_MEANING, while doing: %s\n", process);
                                          printf("stack_t \"%s\"[%p] created by %s at %s:%lu\n", stack->name, stack, stack->func, stack->file, stack->line);
                                          found_error = true;
                                          break;

            case NULL_VALUE_RETURN      : stack_dump(stack, "NULL_VALUE_RETURN", process);
                                          found_error = true;
                                          break;

            case POISON_ELEMENT_MENTION : stack_dump(stack, "POISON_ELEMNET_MENTION", process);
                                          found_error = true;
                                          break;

            case LEFT_CANARY_LOSE       : stack_dump(stack, "LEFT_CANARY_LOSE", process);
                                          found_error = true;
                                          break;

            case RIGHT_CANARY_LOSE      : stack_dump(stack, "RIGHT_CANARY_LOSE", process);
                                          found_error = true;
                                          break;

            case LEFT_STACK_CANARY_LOSE : stack_dump(stack, "LEFT_STACK_CANARY_LOSE", process);
                                          found_error = true;
                                          break;

            case RIGHT_STACK_CANARY_LOSE: stack_dump(stack, "RIGHT_STACK_CANARY_LOSE", process);
                                          found_error = true;
                                          break;

            case HASH_MEANING_CHANGED   : stack_dump(stack, "HASH_MEANING_CHANGED", process);
                                          found_error = true;
                                          break;

            case ENORMOUS_CAPACITY      : PRINT_COLOR(EXTRA_RED, "reason: ENORMOUS_CAPACITY, while doing: %s\n", process);
                                          printf("stack_t \"%s\"[%p] created by %s at %s:%lu\n", stack->name, stack, stack->func, stack->file, stack->line);
                                          found_error = true;
                                          break;

            case SUCCESSFUL_RETURN      : break;

            default                     : PRINT_COLOR(EXTRA_RED, "reason: UNKNOWN_ERROR, while doing: %s\n", process);
                                          found_error = true;
        }
    )

    if (found_error)
        return ABORT_PROCESS;

    else
        return CONTINUE_PROCESS;
}

void poison_stack(size_t begin, size_t end, stack_t *stack)
{

    for (size_t i = begin; i < end; i++)
    {
        stack->data[i] = POISON;
    }
}

error_codes check_allocation(stack_elem_t *data, size_t desirable_size)
{
    size_t malloc_size = malloc_usable_size((void *)data);

    if (data == NULL)
        return NULL_DATA_MEANING;

    if (malloc_size < desirable_size)
    {
        printf("malloc_size = %lu, desirable_size = %lu\n", malloc_size, desirable_size);
        return MEMORY_ALLOCATION_ERROR;
    }

    return SUCCESSFUL_RETURN;
}

size_t djb2(stack_elem_t element)
{
    char string[30] = {};
    sprintf(string, deb_spec "", element);
    size_t sum = 5381;

    for (size_t i = 0; i < 30 && string[i]; i++)
    {
        sum = sum * 33 + (size_t)string[i];
    }

    return sum % 100;
}

size_t hash_eval(stack_t stack)
{
    size_t hash = 0;

    for (size_t i = 1; i < stack.capacity + 1; i++)
    {
        hash += djb2(stack.data[i]);

        if (hash > stack.hash)
            return hash;
    }

    return hash;
}

void hash_set(stack_t *stack)
{
    size_t hash = 0;

    for (size_t i = 1; i < stack->capacity + 1; i++)
    {
        hash += djb2(stack->data[i]);
    }

    stack->hash = hash;
}

error_codes hash_check(stack_t stack)
{
    size_t hash = hash_eval(stack);

    if (hash != stack.hash)
        return HASH_MEANING_CHANGED;

    return SUCCESSFUL_RETURN;
}

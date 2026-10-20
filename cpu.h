#ifndef CPU_H
#define CPU_H
#include "stack.h"


typedef enum commands
{
    CMD_PUSH = 1,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_DIV = 4,
    CMD_OUT = 5,
    CMD_POPr = 6,
    CMD_MUL = 7,
    CMD_JMP = 8,
    CMD_JA = 9,
    CMD_JAE = 10,
    CMD_JB = 11,
    CMD_JBE = 12,
    CMD_JE = 13,
    CMD_JNE = 14,
    CMD_IN = 15,
    CMD_CALL = 16,
    CMD_RET = 17,
    CMD_SQRT = 18,
    CMD_POPm = 19,
    CMD_DRAW = 20,
    CMD_POPmr = 21,
    CMD_HLT = 22,
    CMD_UC= 30
} comands;

typedef enum registers
{
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
    EX = 5,
    FX = 6,
    GX = 7,
    HX = 8,
    IX = 9,
    UR = 666
} registers;

error_codes args_analysis(int argc, const char * const argv[], FILE **fp);

#endif
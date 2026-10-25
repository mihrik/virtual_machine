IN
POP AX

PUSH 0
POP BX

CALL FACTORIAL
OUT
HLT

:FACTORIAL
    PUSH BX; all factorial elements in stack

    PUSH AX
    POP BX

    PUSH AX
    PUSH 1
    JNE NOT_ONE

    POP BX
    PUSH AX
    RET

:NOT_ONE
    PUSH AX
    PUSH 1
    SUB
    POP AX; AX--

    CALL FACTORIAL
    PUSH BX
    MUL
    POP AX
    POP BX
    PUSH AX
    RET

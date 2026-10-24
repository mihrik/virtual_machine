IN
POPr AX

PUSH 0
POPr BX

CALL FACTORIAL
OUT
HLT

:FACTORIAL
    PUSH BX; all factorial elements in stack

    PUSH AX
    POPr BX

    PUSH AX
    PUSH 1
    JNE NOT_ONE

    POPr BX
    PUSH AX
    RET

:NOT_ONE
    PUSH AX
    PUSH 1
    SUB
    POPr AX; AX--

    CALL FACTORIAL
    PUSH BX
    MUL
    POPr AX
    POPr BX
    PUSH AX
    RET

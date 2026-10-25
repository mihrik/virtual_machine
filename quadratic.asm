IN
POP AX; AX = a
IN
POP BX; BX = b
IN
POP CX; CX = c
CALL COEF_AN; coefficients analysis
CALL DISCRIMINANT; next line we got discriminant
CALL CHECK_DS; check discriminant
OUT
HLT

;                                       DISCRIMINANT EVALUATION
:DISCRIMINANT
    PUSH BX
    PUSH BX
    MUL; b^2 in stack
    PUSH 4
    PUSH AX
    PUSH CX
    MUL; ac in stack
    MUL; 4ac in stack
    SUB; b^2 - 4ac in stack
    POP DX; DX = sqrt(d)
RET

;                                       DISCRIMINANT ANALYSIS
:CHECK_DS
    PUSH DX
    PUSH 0
    JE ZERO_DS

    PUSH DX
    PUSH 0
    JA DS_ABOVE_ZERO

    PUSH DX
    PUSH 0
    JB DS_BELOW_ZERO

RET

;                                       0 DISCRIMINANT HANDLE
:ZERO_DS
    PUSH 0
    PUSH BX
    SUB; got -b
    PUSH 2
    DIV; got -b / 2
    PUSH AX
    DIV; got -b / 2a
RET

;                                       ABOVE 0 DISCRIMINANT HANDLE
:DS_ABOVE_ZERO
    PUSH DX
    SQRT
    POP DX; sqrt(d)
    PUSH 0
    PUSH BX
    SUB; got -b
    PUSH DX
    ADD; -b + sqrt(d)
    PUSH 2
    DIV; (-b + sqrt(d)) / 2
    PUSH AX
    DIV; (-b + sqrt(d)) / 2a
    OUT
    PUSH 0
    PUSH DX
    SUB; got -d
    PUSH 0
    PUSH BX
    SUB
    ADD; -b - sqrt(d)
    PUSH 2
    DIV; (-b - sqrt(d)) / 2
    PUSH AX
    DIV; (-b - sqrt(d)) / 2a
RET

;                                       BELOW 0 DISCRIMINANT HANDLE
:DS_BELOW_ZERO
    PUSH 666
    OUT
    HLT

;                                       COEFFICIENTS ANALYSIS
:COEF_AN
    PUSH AX
    PUSH 0
    JE AX_ZERO
RET

;                                       AX = 0 HANDLE
:AX_ZERO
    PUSH BX
    PUSH 0
    JE AX_AND_BX_ZERO
    PUSH 0
    PUSH CX
    SUB; got -c
    PUSH BX
    DIV; -c/b
    OUT
    HLT

;                                       AX = BX = 0 HANDLE
:AX_AND_BX_ZERO
    PUSH CX
    PUSH 0
    JNE DS_BELOW_ZERO
    PUSH 888
    OUT
    HLT
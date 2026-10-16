IN
POPr AX; AX = a
IN
POPr BX; BX = b
IN
POPr CX; CX = c
CALL 6; coefficients analysis
CALL 1; next line we got discriminant
CALL 2; check discriminant
OUT
HLT

;                                       DISCRIMINANT EVALUATION
:1
PUSHr BX
PUSHr BX
MUL; b^2 in stack
PUSH 4
PUSHr AX
PUSHr CX
MUL; ac in stack
MUL; 4ac in stack
SUB; b^2 - 4ac in stack
POPr DX; DX = sqrt(d)
RET

;                                       DISCRIMINANT ANALYSIS
:2
PUSHr DX
PUSH 0
JE 3

PUSHr DX
PUSH 0
JA 4

PUSHr DX
PUSH 0
JB 5

RET

;                                       0 DISCRIMINANT HANDLE
:3
PUSH 0
PUSHr BX
SUB; got -b
PUSH 2
DIV; got -b / 2
PUSHr AX
DIV; got -b / 2a
RET

;                                       ABOVE 0 DISCRIMINANT HANDLE
:4
PUSHr DX
SQRT
POPr DX; sqrt(d)
PUSH 0
PUSHr BX
SUB; got -b
PUSHr DX
ADD; -b + sqrt(d)
PUSH 2
DIV; (-b + sqrt(d)) / 2
PUSHr AX
DIV; (-b + sqrt(d)) / 2a
OUT
PUSH 0
PUSHr DX
SUB; got -d
PUSH 0
PUSHr BX
SUB
ADD; -b - sqrt(d)
PUSH 2
DIV; (-b - sqrt(d)) / 2
PUSHr AX
DIV; (-b - sqrt(d)) / 2a
RET

;                                       BELOW 0 DISCRIMINANT HANDLE
:5
PUSH 666
OUT
HLT

;                                       COEFFICIENTS ANALYSIS
:6
PUSHr AX
PUSH 0
JE 7
RET

;                                       AX = 0 HANDLE
:7
PUSHr BX
PUSH 0
JE 5
PUSH 0
PUSHr CX
SUB; got -c
PUSHr BX
DIV; -c/b
OUT
HLT
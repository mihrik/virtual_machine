IN
POPr AX; AX = a
IN
POPr BX; BX = b
IN
POPr CX; CX = c
CALL 1; next line we got discriminant
PUSHr DX
PUSH 0
CALL 2
OUT
HLT

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
SQRT
POPr DX; DX = sqrt(d)
RET

:2
JE 3
RET
:3
PUSH 0
PUSHr BX
SUB; got -b
PUSH 2
DIV; got -b / 2
PUSHr AX
DIV; got -b / 2a


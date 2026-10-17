;                           ELLIPSE VALUES PREPARING
IN; odd number
POPr AX; AX = m
PUSHr AX
PUSH 2
DIV; got m / 2
PUSH 1
ADD; got m / 2 + 1
POPr BX
CALL 1
; TODO
DRAW
HLT

;                           ELLIPSE DRAWING
;                          NECESSARY VALUES
;                       BX = m/2 + 1 = n, AX = m
:1
PUSHr BX
PUSH 1
SUB; n - 1
PUSH 2
DIV; (n-1)/2
POPr CX; CX = (n-1)/2
PUSHr AX
PUSH 1
SUB; m - 1
PUSH 2
DIV
POPr DX; DX = (m-1)/2
;                       WE HAVE AX = m, BX = n, CX = (n-1)/2, DX = (m-1)/2
;                       FIRST 'FOR' BEGINNING
PUSH 0
POPr EX;                    EX = 0 = i
;                       SECOND 'FOR' BEGINNING
:2
PUSH 0
POPr FX;                    FX = 0 = j
:3
;                       MAIN ACTIONS

PUSHr FX
PUSHr DX
SUB
POPr GX;                    GX = j - hor_radius = x

PUSHr CX
PUSHr EX
SUB
POPr HX;                    HX = vert_radius - i = y

PUSHr GX
PUSHr GX
MUL; x * x
PUSHr CX
PUSHr CX
MUL; vert_radius * vert_radius
MUL; vert_radius * vert_radius * x * x
PUSHr HX
PUSHr HX
MUL; y * y
PUSHr DX
PUSHr DX
MUL; hor_radius * hor_radius
MUL; hor_radius * hor_radius * y * y
ADD; hor_radius * hor_radius * y * y + x * x * vert_radius * vert_radius
PUSHr DX
PUSHr DX
MUL; hor_radius * hor_radius
PUSHr CX
PUSHr CX
MUL; vert_radius * vert_radius
MUL; vert_radius * vert_radius * hor_radius * hor_radius
CALL 4

;                       END OF MAIN ACTIONS
PUSHr FX
PUSH 1
ADD
POPr FX; FX++
PUSHr FX
PUSHr AX
JB 3; if j < m
;                       SECOND 'FOR' END
PUSHr EX
PUSH 1
ADD
POPr EX; EX++
PUSHr EX
PUSHr BX
JB 2; if i < n
RET
;                       FIRST FOR END

;                       FILL RAM
:4
JBE 5
PUSHr EX
PUSHr AX
MUL; i * m
PUSHr FX
ADD; i * n + j
POPr IX;                    IX = i * n + j
PUSH 46
POPmr IX
RET

:5
PUSHr EX
PUSHr AX
MUL; i * m
PUSHr FX
ADD; i * m + j
POPr IX
PUSH 35
POPmr IX
RET
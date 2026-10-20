;                           ELLIPSE VALUES PREPARING
IN; odd number
POPr AX; AX = m
PUSH AX
PUSH 2
DIV; got m / 2
PUSH 1
ADD; got m / 2 + 1
POPr BX
CALL EVAL
;
DRAW
HLT

;                           ELLIPSE DRAWING
;                          NECESSARY VALUES
;                       BX = m/2 + 1 = n, AX = m
:EVAL
PUSH BX
PUSH 1
SUB; n - 1
PUSH 2
DIV; (n-1)/2
POPr CX; CX = (n-1)/2
PUSH AX
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
:FIRST_LOOP
PUSH 0
POPr FX;                    FX = 0 = j
:SECOND_LOOP
;                       MAIN ACTIONS

PUSH FX
PUSH DX
SUB
POPr GX;                    GX = j - hor_radius = x

PUSH CX
PUSH EX
SUB
POPr HX;                    HX = vert_radius - i = y

PUSH GX
PUSH GX
MUL; x * x
PUSH CX
PUSH CX
MUL; vert_radius * vert_radius
MUL; vert_radius * vert_radius * x * x
PUSH HX
PUSH HX
MUL; y * y
PUSH DX
PUSH DX
MUL; hor_radius * hor_radius
MUL; hor_radius * hor_radius * y * y
ADD; hor_radius * hor_radius * y * y + x * x * vert_radius * vert_radius
PUSH DX
PUSH DX
MUL; hor_radius * hor_radius
PUSH CX
PUSH CX
MUL; vert_radius * vert_radius
MUL; vert_radius * vert_radius * hor_radius * hor_radius
CALL FILL_RAM

;                       END OF MAIN ACTIONS
PUSH FX
PUSH 1
ADD
POPr FX; FX++
PUSH FX
PUSH AX
JB SECOND_LOOP; if j < m
;                       SECOND 'FOR' END
PUSH EX
PUSH 1
ADD
POPr EX; EX++
PUSH EX
PUSH BX
JB FIRST_LOOP; if i < n
RET
;                       FIRST FOR END

;                       FILL RAM
:FILL_RAM
JBE IN_CIRC
PUSH EX
PUSH AX
MUL; i * m
PUSH FX
ADD; i * n + j
POPr IX;                    IX = i * n + j
PUSH 46
POPmr IX
RET

:IN_CIRC
PUSH EX
PUSH AX
MUL; i * m
PUSH FX
ADD; i * m + j
POPr IX
PUSH 35
POPmr IX
RET
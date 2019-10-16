.ORIG x3000

LEA     R0, k               ;3000
LDW     R0, R0, #0          ;3002

NOT     R1, R0              ;3004
XOR     R2, R0, xe          ;3006
LSHF    R3, R0, #12         ;3008
RSHFL   R4, R3, #7          ;300A
RSHFA   R5, R3, #4          ;300C

LEA R0, a
LDW R0, R0, #0
LEA R1, b
LDW R1, R1, #0
ADD R2, R0, R1
AND R3, R0, R1
AND R4, R0, x3

HALT                        ;300E
k .FILL xf
a .FILL x11
b .FILL x12
.END
;NOT
;XOR
;LSHF
;RSHFL
;RSHFA


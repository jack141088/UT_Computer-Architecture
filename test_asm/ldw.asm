.ORIG X3000
LEA     R5, k               ;3000
LDW     R5, R5, #3          ;3002
LEA     R1, NUM             ;3004
LDW     R1, R1, #0          ;3006
STW     R5, R1, #-2          ;3008

LEA     R0, GG              ;300a
LDW     R3, R0, #-1          ;300c

HALT                        ;300e
NUM     .FILL x4000         ;3010
K .FILL #0                 ;3012
GG .FILL #-1                ;3014
TT .FILL #-4                ;3016
T2 .FILL #-8                ;3016
.END

.ORIG X3000
LEA     R5, k               ;3000
LDB     R5, R5, #3          ;3002
LEA     R1, NUM             ;3004
LDW     R1, R1, #0          ;3006
STB     R5, R1, #-3          ;3008
STB     R5, R1, #2          ;3008
STB     R5, R1, #0          ;3008

LEA     R0, GG              ;300a
LDB     R3, R0, #-2          ;300c

HALT                        ;300e
NUM     .FILL x4000         ;3010
K .FILL x7f                 ;3012
GG .FILL x3f11             ;3014
TT .FILL #-4                ;3016
T2 .FILL #-8                ;3016
.END

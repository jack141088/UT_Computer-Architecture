.ORIG X3000
LEA     R5, k               ;3000
LDW     R5, R5, #0          ;3002
LEA     R1, NUM             ;3004
LDW     R1, R1, #0          ;3006
STW     R5, R1, #0          ;3008
AND R0, R0, #0              ;300A
LEA R3, NUM                 ;300C
LDW R3, R3, #0              ;300E
LDW R1, R3, #0              ;3010
ADD R2, R1, #0              ;3012
LOOP    ADD R0, R0, R1      ;3014
ADD R2, R2, #-1             ;3016
BRP LOOP                    ;3018
STW R0, R3, #1              ;301A
HALT                        ;301C
NUM     .FILL x4000         ;301E
K .FILL #10                 ;3020
.END

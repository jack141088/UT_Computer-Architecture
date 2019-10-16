.ORIG X3000
LEA     R5, k               ;3000
LDW     R5, R5, #0          ;3002
LEA     R1, NUM             ;3004
LDW     R1, R1, #0          ;3006
STW     R5, R1, #0          ;3008       //Store k #10 into address x4000

LEA     R4, Store           ;300A
LDW     R4, R4, #0          ;300C       //Store x3028 into Reg 4

AND R0, R0, #0              ;300E
LEA R3, NUM                 ;3010
LDW R3, R3, #0              ;3012
LDW R1, R3, #0              ;3014
ADD R2, R1, #0              ;3016
LOOP    ADD R0, R0, R1      ;3018
ADD R2, R2, #-1             ;301A
BRP LOOP                    ;301C
JSRR R4                     ;301E
HALT                        ;3020
HALT                        ;3022
HALT                        ;3024
HALT                        ;3026
T STW R0, R3, #1            ;3028
HALT                        ;302A
NUM     .FILL x4000         ;302C
K .FILL #10                 ;302E
Store .FILL x3028           ;3030
.END

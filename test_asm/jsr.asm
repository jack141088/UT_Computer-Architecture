.ORIG x3000
LEA     R0, data0           ;3000
LDW     R0, R0, #0          ;3002
LEA     R1, data1           ;3004
LDW     R1, R1, #0          ;3006


AND r2, R0, r1              ;3008
AND R3, R0, x-1             ;300a cc => N = 1
JSRR R1                     ;300c
JSR label1                  ;300e
halt                        ;3010
halt                        ;3012
label0 add r4, r1, x0       ;3014
RET                         ;3016
label1 add r5, r0, x1       ;3018
HALT                        ;301a
data0 .FILL xFFFF           ;301c 
data1 .FILL x3014                 
.END

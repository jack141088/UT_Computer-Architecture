.ORIG x3000
LEA     R0, data0           ;3000
LDW     R0, R0, #0          ;3002
LEA     R1, data1           ;3004
LDW     R1, R1, #0          ;3006


AND r2, R0, r1              ;3008
AND R3, R0, x-1             ;300a cc => N = 1
JMP R1                      ;300c
halt                        ;300e
halt                        ;3010
label0 add r4, r1, x0       ;3012
HALT                        ;300c
data0 .FILL xFFFF           ;300e 
data1 .FILL x3012                 
.END

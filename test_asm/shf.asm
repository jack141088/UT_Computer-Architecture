.ORIG x3000
LEA     R0, data0           ;3000
LDW     R0, R0, #0          ;3002
LEA     R1, data1           ;3004
LDW     R1, R1, #0          ;3006


LSHF r2, R0, x3              ;3008
RSHFL r3, r0, xf
LSHF r3, r3, xf
RSHFA r4, r3, x8
HALT                        ;300c
data0 .FILL x-8           ;300e 
data1 .FILL x1                 
.END

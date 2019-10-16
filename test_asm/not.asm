.ORIG x3000
LEA     R0, data0           ;3000
LDW     R0, R0, #0          ;3002

NOT r2, R0                  ;3008
HALT                        ;300c
data0 .FILL xFF           ;300e 
.END

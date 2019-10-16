.ORIG x30
LEA     R0, data0           ;30
LDW     R0, R0, #0          ;32
LEA     R1, data1           ;34
LDW     R1, R1, #0          ;36

;TRAP x1e                    ;38
TRAP x23                    ;38
HALT                        ;3c
data0 .FILL xffff           ;3e 
data1 .FILL x1401           ;40
.END

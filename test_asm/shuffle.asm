.ORIG x2000
LEA     R5, data1           ;1000
LDW     R5, R5, #0          ;1002
LEA     R1, fill1           ;1004
LDW     R1, R1, #0          ;1006
STW     R5, R1, #0          ;1008

LEA     R5, data2           ;100A
LDW     R5, R5, #0          ;100C
LEA     R1, fill2           ;100E
LDW     R1, R1, #0          ;1010
STW     R5, R1, #0          ;1012

LEA     R5, data3           ;1014
LDW     R5, R5, #0          ;1016
LEA     R1, fill3           ;1018
LDW     R1, R1, #0          ;101A
STW     R5, R1, #0          ;101C

LEA     R5, data4           ;101E
LDW     R5, R5, #0          ;1020
LEA     R1, fill4           ;1022
LDW     R1, R1, #0          ;1024
STW     R5, R1, #0          ;1026

LEA     R5, data5           ;101E
LDW     R5, R5, #0          ;1020
LEA     R1, fill5           ;1022
LDW     R1, R1, #0          ;1024
STW     R5, R1, #0          ;1026

LEA     R5, data6           ;101E
LDW     R5, R5, #0          ;1020
LEA     R1, fill6           ;1022
LDW     R1, R1, #0          ;1024
STW     R5, R1, #0          ;1026

LEA     R5, data7           ;101E
LDW     R5, R5, #0          ;1020
LEA     R1, fill7           ;1022
LDW     R1, R1, #0          ;1024
STW     R5, R1, #0          ;1026

LEA     R5, data8           ;101E
LDW     R5, R5, #0          ;1020
LEA     R1, fill8           ;1022
LDW     R1, R1, #0          ;1024
STW     R5, R1, #0          ;1026

AND     R0, R0, #0
ADD     R0, R0, #8

LEA R5, First
LDW R1, R5, #0 ;R1 = x3050

START 	LDW R2, R1, #0 ;Store (*R1)->R2
	LDB R3, R1, #1 ;Store *(R1+1)->R3
	STB R3 R1 #0
	STB R2 R1 #1
       	ADD R1, R1, #2 ;Address +2
	ADD R0, R0, #-1
       	BRZ DONE
       	BR START
DONE TRAP x25 
First 	.FILL x3050
fill1 .FILL x3050
fill2 .FILL x3052
fill3 .FILL x3054
fill4 .FILL x3056
data1 .FILL x0001
data2 .FILL x100A
data3 .FILL x7045
data4 .FILL x7BCD
fill5 .FILL x3058
fill6 .FILL x305A
fill7 .FILL x305C
fill8 .FILL x305E
data5 .FILL x1234
data6 .FILL x7bcd
data7 .FILL x0019
data8 .FILL xE050
.End

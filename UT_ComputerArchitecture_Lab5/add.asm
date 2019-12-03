            .ORIG   x3000
        ; Register Initialization
            AND     R1, R1, #0      ; R1 = TEMP SUM = 0
            LEA     R2, COUNT
            LDW     R2, R2, #0      ; R2 = COUNT = 20
            LEA     R0, DATAINIT
            LDW     R0, R0, #0      ; R0 = Initial location = xC000
        ; Loop
LOOP        LDB     R3, R0, #0      ; R3 = MEM[R0] (byte)
            ADD     R1, R1, R3      ; R1 = TEMP SUM += MEM[R0]
            ADD     R0, R0, #1      ; R0(Location) Increment by 1
            ADD     R2, R2, #-1     ; R2(Count) Decrement by 1
            BRp     LOOP            ; if (R2 > 0): go LOOP; else: Continue
        ; Store Final Result at xC014
            LEA     R0, DEST
            LDW     R0, R0, #0
            STW     R1, R0, #0
            JMP     R1
            HALT
DATAINIT    .FILL   xC000
COUNT       .FILL   #20
;DEST        .FILL   xC015           ; unaligned exception
;DEST        .FILL   xD000           ; pagefault exception
;DEST        .FILL   x2000           ; protection exception
DEST        .FILL   xC014
            .END

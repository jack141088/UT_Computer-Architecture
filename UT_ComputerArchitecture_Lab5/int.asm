            .ORIG   x1200
        ; Store R0, R1, R2
            STW     R0, R6, #-1
            STW     R1, R6, #-2
            STW     R2, R6, #-3
        ; Page Table Entry Iteration Pre-processing
            LEA     R0, PTADDR
            LDW     R0, R0, #0      ; R0 = Page Table Start Address
            LEA     R1, ENTRY
            LDW     R1, R1, #0      ; R1 = Counts of Entries
        ; Iteration
LOOP        LDW     R2, R0, #0
            AND     R2, R2, #-2
            STW     R2, R0, #0
            ADD     R0, R0, #2
            ADD     R1, R1, #-1
            BRp     LOOP
        ; Restore R0, R1, R2
            LDW     R0, R6, #-1
            LDW     R1, R6, #-2
            LDW     R2, R6, #-3
            RTI
PTADDR      .FILL   x1000
ENTRY       .FILL   x0080
            .END

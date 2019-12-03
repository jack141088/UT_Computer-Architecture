            .ORIG   x1200
        ; Store R0, R1
            STW     R0, R6, #-1
            STW     R1, R6, #-2
        ; Location Increment
            LEA     R0, MEMLOCATION
            LDW     R0, R0, #0      ; R0 = Initial Location = x4000
            LDW     R1, R0, #0      ; R1 = MEM[x4000]
            ADD     R1, R1, #1      ; R1 += 1
            STW     R1, R0, #0      ; MEM[x4000] = R1
        ; Restore R0, R1
            LDW     R0, R6, #-1
            LDW     R1, R6, #-2
            RTI
MEMLOCATION .FILL   x4000
            .END

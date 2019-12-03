gcc -std=c99 -o simulate lc3bsim5.c
./assembler.linux add.asm add
./simulate ucode5 pagetable add data vector_table int except_prot except_unaligned except_unknown except_pagefault

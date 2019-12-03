gcc -std=c99 -o simulate lc3bsim5.c
./assembler.linux ../test_asm/$1.asm ../test_asm/$1
./simulate ucode5 pagetable ../test_asm/$1 vector_table int except_prot except_unaligned except_unknown except_pagefault

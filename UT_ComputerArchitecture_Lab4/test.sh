gcc -std=c99 -o simulate lc3bsim4.c
./assembler.linux ../test_asm/$1.asm ../test_asm/$1
./simulate ucode4 ../test_asm/$1 vector_table int except_prot except_unaligned except_unknown

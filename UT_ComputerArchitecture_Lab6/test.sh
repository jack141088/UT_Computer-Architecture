gcc -std=c99 -o simulate lc3bsim6.c
./assembler.linux ../test_asm/$1.asm ../test_asm/$1
./simulate ucode6 ../test_asm/$1

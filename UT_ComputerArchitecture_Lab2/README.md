# UT_ComputerArchitecture_Lab2
Computer Architecture Lab2

## Introduction
For this assignment, you will write an instruction-level simulator for the LC-3b. The simulator will take one input file entitled isaprogram, which is an assembled LC-3b program.
The simulator will execute the input LC-3b program, one instruction at a time, modifying the architectural state of the LC-3b after each instruction.

## Usage
```
gcc -lm -std=c99 -o simulate lc3bsim2.c
./assembler.linux [file.asm] output.txt
./simulate output.txt
```

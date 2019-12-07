# UT ComputerArchitecture Lab3
Computer Architecture Lab3

## Introduction
For this assignment, you will write a cycle-level simulator for the LC-3b. The simulator will take two input files:
A file entitled ucode3 which holds the control store.
A file entitled isaprogram which is an assembled LC-3b program.
The simulator will execute the input LC-3b program, using the microcode to direct the simulation of the microsequencer, datapath, and memory components of the LC-3b.

## Usage
```
./assembler.linux [file.asm] ./isaprogram
gcc -std=c99 -o simulate lc3bsim3.c
./simulate ucode3 isaprogram
```

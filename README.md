# UT_Computer-Architecture
Computer Architecture Lab

## Introduction

### Lab1
The purpose of this lab is to reinforce the concepts of assembly language and assemblers. In this lab assignment, you will write an LC-3b Assembler, whose job will be to translate assembly language source code into the machine language (ISA) of the LC-3b. You will also write a program to solve a problem in the LC-3b Assembly Language.

### Lab2
For this assignment, you will write an instruction-level simulator for the LC-3b. The simulator will take one input file entitled isaprogram, which is an assembled LC-3b program.
The simulator will execute the input LC-3b program, one instruction at a time, modifying the architectural state of the LC-3b after each instruction.

### Lab3
For this assignment, you will write a cycle-level simulator for the LC-3b. The simulator will take two input files:
A file entitled ucode3 which holds the control store.
A file entitled isaprogram which is an assembled LC-3b program.
The simulator will execute the input LC-3b program, using the microcode to direct the simulation of the microsequencer, datapath, and memory components of the LC-3b.

### Lab4
The goal of this lab assignment is to extend the LC-3b simulator you wrote in Lab 3 to handle interrupts and exceptions. You will augment the existing LC-3b microarchitecture to support detection and handling of one type of interrupts (timer) and three types of exceptions (protection, unaligned access, and unknown opcode).

### Lab5
The goal of this lab assignment is to extend the LC-3b simulator you wrote in Lab 4 to handle virtual memory. You will augment the existing LC-3b microarchitecture in order to support virtual to physical address translation. You will also provide microarchitectural support for page fault exceptions and change the protection exception from Lab 4 to be based on access mode in the PTE.

### Lab6
For this assignment, you will write a simulator for the pipelined LC-3b. The simulator will take two input files:

A file entitled ucode which holds the control store that is located in the DE stage of the pipeline.

A file entitled isaprogram holding an assembled LC-3b program.

The simulator will execute the input LC-3b program using the control store and the code you write inside the simulator functions to direct the simulation of the datapath and memory components of the LC-3b.


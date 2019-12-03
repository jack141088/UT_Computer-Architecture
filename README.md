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

The inputs to this new simulator will be:

A file entitled ucode4 which will hold the expanded microcontrol store.

A file holding an LC-3b user program that will be loaded into memory starting at memory location x3000.

A file holding data for the user program. This data will be loaded into memory starting at memory location xC000.

A file holding the interrupt/exception vector table that will be loaded into memory starting at memory location x0200. You should form the contents of this table based on the vector of each interrupt/exception and starting address of the service routine for each interrupt/exception.

A file holding an LC-3b system program that will be loaded into memory starting at memory location x1200. This will be the timer interrupt service routine.

A file holding an LC-3b system program that will be loaded into memory starting at memory location x1600. This will be the protection exception handler.

A file holding an LC-3b system program that will be loaded into memory starting at memory location x1A00. This will be the unaligned access exception handler.

A file holding an LC-3b system program that will be loaded into memory starting at memory location x1C00. This will be the unknown opcode exception handler.

The simulator will start executing the LC-3b program loaded at x3000. The timer interrupt will occur at cycle 300. You are supposed to provide microarchitectural support for interrupt handling. The interrupt handler you write should increment location (word-sized access) x4000 by 1. You will also need to implement the RTI instruction so that the interrupt handler can transfer control back to the user program.
You will have to provide microarchitectural support for handling exceptions. Most of this support is similar to the support you will add for interrupts. If the user program accesses a memory location that can only be accessed in supervisor mode, a protection exception will occur. The exception service routine you will write should simply halt the machine. If the user program makes an unaligned memory access in either supervisor mode or user mode, an unaligned access exception will occur. The exception service routine again should simply halt the machine. If the user program tries to use an unknown opcode (1010 or 1011), an unknown opcode exception will occur. The exception service routine again should simply halt the machine. (Important note: we should be able to swap out your exception routines with a routine that returns from an exception. Therefore, make sure you implement the exceptions correctly.)


### Lab5
The goal of this lab assignment is to extend the LC-3b simulator you wrote in Lab 4 to handle virtual memory. You will augment the existing LC-3b microarchitecture in order to support virtual to physical address translation. You will also provide microarchitectural support for page fault exceptions and change the protection exception from Lab 4 to be based on access mode in the PTE.

### Lab6
For this assignment, you will write a simulator for the pipelined LC-3b. The simulator will take two input files:

A file entitled ucode which holds the control store that is located in the DE stage of the pipeline.

A file entitled isaprogram holding an assembled LC-3b program.

The simulator will execute the input LC-3b program using the control store and the code you write inside the simulator functions to direct the simulation of the datapath and memory components of the LC-3b.


# UT_ComputerArchitecture_Lab6
Computer Architecture Lab6

## Introduction
For this assignment, you will write a simulator for the pipelined LC-3b. The simulator will take two input files:

A file entitled ucode which holds the control store that is located in the DE stage of the pipeline.

A file entitled isaprogram holding an assembled LC-3b program.

The simulator will execute the input LC-3b program using the control store and the code you write inside the simulator functions to direct the simulation of the datapath and memory components of the LC-3b.

## Usage

### generate lab6 example binary code
```
./gen_asm.sh
```

### run lab6 example
```
./run.sh example[number]
```

### run test asm
```
./test.sh [filename]
```

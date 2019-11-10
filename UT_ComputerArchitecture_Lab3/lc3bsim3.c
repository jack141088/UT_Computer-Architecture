/*
    Name 1: Ang Hsuan Lee
    UTEID 1: al48944
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS {                                                  
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
        (x[J3] << 3) + (x[J2] << 2) +
        (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return(x[DRMUX]); }
int GetSR1MUX(int *x)        { return(x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
 *    MEMORY[A][1] stores the most significant byte of word at word address A 
 *       There are two write enable signals, one for each byte. WE0 is used for 
 *          the least significant byte of a word. WE1 is used for the most significant 
 *             byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;    /* run bit */
int BUS;    /* value of the bus */

typedef struct System_Latches_Struct{

    int PC,     /* program counter */
        MDR,    /* memory data register */
        MAR,    /* memory address register */
        IR,     /* instruction register */
        N,      /* n condition bit */
        Z,      /* z condition bit */
        P,      /* p condition bit */
        BEN;        /* ben register */

    int READY;  /* ready bit */
    /* The ready bit is also latched as you dont want the memory system to assert it 
     *      at a bad point in the cycle*/

    int REGS[LC_3b_REGS]; /* register file. */

    int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

    int STATE_NUMBER; /* Current State Number - Provided for debugging */ 
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

    eval_micro_sequencer();   
    cycle_memory();
    eval_bus_drivers();
    drive_bus();
    latch_datapath_values();

    CURRENT_LATCHES = NEXT_LATCHES;

    CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%.4x\n", BUS);
    printf("MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
        printf("Error: Can't open micro-code file %s\n", ucode_filename);
        exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
        if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
            printf("Error: Too few lines (%d) in micro-code file: %s\n",
                    i, ucode_filename);
            exit(-1);
        }

        /* Put in bits one at a time. */
        index = 0;

        for (j = 0; j < CONTROL_STORE_BITS; j++) {
            /* Needs to find enough bits in line. */
            if (line[index] == '\0') {
                printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                        ucode_filename, i);
                exit(-1);
            }
            if (line[index] != '0' && line[index] != '1') {
                printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                        ucode_filename, i, j);
                exit(-1);
            }

            /* Set the bit in the Control Store. */
            CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
            index++;
        }

        /* Warn about extra bits in line. */
        if (line[index] != '\0')
            printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                    ucode_filename, i);
    }
    printf("\n");
}

/************************************************************/
/*                                                          */
/* Procedure : init_memory                                  */
/*                                                          */
/* Purpose   : Zero out the memory array                    */
/*                                                          */
/************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                    program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) { 
    int i;
    init_control_store(ucode_filename);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
        printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
                argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
    You are allowed to use the following global variables in your
    code. These are defined above.
 
    CONTROL_STORE
    MEMORY
    BUS

    CURRENT_LATCHES
    NEXT_LATCHES

    You may define your own local/global variables and functions.
    You may use the functions to get at the control bits defined
    above.

    Begin your code here                       */
/***************************************************************/
int print_show = 0;

void eval_micro_sequencer() {

    /* 
        Evaluate the address of the next state according to the 
        micro sequencer logic. Latch the next microinstruction.
    */
    int IRD = GetIRD(CURRENT_LATCHES.MICROINSTRUCTION);
    int COND = GetCOND(CURRENT_LATCHES.MICROINSTRUCTION);
    int J = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);

    int next_state = 0;
    if (IRD == 0)
    {
        if(COND == 1 & CURRENT_LATCHES.READY == 1)
            next_state = J | 0x2;
        else if(COND == 2 & CURRENT_LATCHES.BEN == 1)
            next_state = J | 0x4;
        else if(COND == 3 & ((CURRENT_LATCHES.IR & 0x00000800) >> 11) == 1)
            next_state = J | 0x1;
        else next_state = J;
    }
    else if (IRD == 1)
    {
        next_state = (CURRENT_LATCHES.IR & 0x0000F000) >> 12;
    }
    
    memcpy(NEXT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[next_state], sizeof(int)*CONTROL_STORE_BITS);
    NEXT_LATCHES.STATE_NUMBER = next_state;
    
    if(print_show)
    {
        printf("\neval micro sequencer:\n");
        printf("CURRENT_STATE: %d\n", CURRENT_LATCHES.STATE_NUMBER);
        printf("NEXT_STATE: %d\n", next_state);
    }
}

int cycleCtr = 0;

void cycle_memory() {

    /* 
       This function emulates memory and the WE logic. 
       Keep track of which cycle of MEMEN we are dealing with.  
       If fourth, we need to latch Ready bit at the end of 
       cycle to prepare microsequencer for the fifth cycle.  
       */
    int MIO_EN = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
    int R_W = GetR_W(CURRENT_LATCHES.MICROINSTRUCTION);
    int DATA_SIZE = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
    int MAR = CURRENT_LATCHES.MAR;
    int MDR = CURRENT_LATCHES.MDR;

    if(print_show)
    {
        printf("\ncycle memory:\n");
        printf("cycle: %d\n", cycleCtr);
        printf("MIO_EN: %d\n", MIO_EN);
        printf("R_W: %d\n", R_W);
        printf("MAR: %x\n", MAR);
        printf("MDR: %x\n", MDR);
    }

    NEXT_LATCHES.READY = 0;
    if (MIO_EN == 0)
    {
        cycleCtr = 0;
        return;
    }
    else if (MIO_EN == 1)
    {
        if (cycleCtr < MEM_CYCLES - 2)
        {
            cycleCtr = cycleCtr + 1;
            return;
        }
        else if (cycleCtr == MEM_CYCLES - 2)
        {
            cycleCtr = cycleCtr + 1;
            NEXT_LATCHES.READY = 1;
        }
        else if (CURRENT_LATCHES.READY == 1)
        {
            if (R_W == 0)    //Read
            {
                CURRENT_LATCHES.MDR = Low16bits( (MEMORY[MAR >> 1][1] << 8) + MEMORY[MAR >> 1][0]);
            }
            else if (R_W == 1)  //Write
            {
                if (DATA_SIZE == 0) //Byte
                {
                    if ((MAR % 2) == 0)
                        MEMORY[MAR >> 1][0] = Low16bits(MDR & 0x00FF);
                    else
                        MEMORY[MAR >> 1][1] = Low16bits(MDR >> 8);
                }
                else if(DATA_SIZE == 1) //Word
                {
                    MEMORY[MAR >> 1][0] = Low16bits(MDR & 0x00FF);
                    MEMORY[MAR >> 1][1] = Low16bits(MDR >> 8);
                }
            }
        }
    }
    if(print_show)
    {
        printf("READ: CURRENT.MDR: %X\n", CURRENT_LATCHES.MDR);
        printf("WRITE: MEMORY: %X", MEMORY[MAR>>1][1]);
        printf("%X\n", MEMORY[MAR>>1][0]);
    }
}

int MARMUX_output, PC_output, ALU_output, SHF_output, MDR_output;
int ADDR2MUX_output, ADDR1MUX_output, ADDER_output;

void eval_bus_drivers() {
    /* 
       Datapath routine emulating operations before driving the bus.
       Evaluate the input of tristate drivers 
       Gate_MARMUX,
       Gate_PC,
       Gate_ALU,
       Gate_SHF,
       Gate_MDR.
       */    
    int MARMUX = GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int ADDR1MUX = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int ADDR2MUX = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int SR1MUX = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);    
    //int PCMUX = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int ALUK = GetALUK(CURRENT_LATCHES.MICROINSTRUCTION);
    int DATA_SIZE = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
    int LSHF1 = GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION);

    int IR = CURRENT_LATCHES.IR;
    int PC = CURRENT_LATCHES.PC;
    int* REGS = CURRENT_LATCHES.REGS;
    int MAR0 = (CURRENT_LATCHES.MAR & 0x00000001);
    int MDR = CURRENT_LATCHES.MDR;

    int IR5 = (IR & 0x00000020) >> 5;
    int IR4 = (IR & 0x00000010) >> 4;
    int SR1 = SR1MUX == 0 ? ((IR & 0x00000E00) >> 9) : ((IR & 0x000001C0) >> 6);
    int SR2 = (IR & 0x00000007);
    int A = REGS[SR1];
    int B = IR5 == 0 ? REGS[SR2] : 
        (((IR & 0x00000010) >> 4) == 0 ? (IR & 0x0000001F) : (IR | 0xFFFFFFE0));
    int amount4 = (IR & 0x0000000F);

    // MARMUX
    if(ADDR1MUX == 0)
        ADDR1MUX_output = PC;
    else if (ADDR1MUX == 1)
        ADDR1MUX_output = A;

    if(ADDR2MUX == 0)
        ADDR2MUX_output = 0;
    else if(ADDR2MUX == 1)
    {
        ADDR2MUX_output = (IR & 0x0000003F);
        if (((ADDR2MUX_output & 0x00000020) >> 5) == 1)
            ADDR2MUX_output = ADDR2MUX_output | 0xFFFFFFC0;
    }
    else if(ADDR2MUX == 2)
    {
        ADDR2MUX_output = (IR & 0x000001FF);
        if (((ADDR2MUX_output & 0x00000100) >> 8) == 1)
            ADDR2MUX_output = ADDR2MUX_output | 0xFFFFFE00;
    }
    else if(ADDR2MUX == 3)
    {
        ADDR2MUX_output = (IR & 0x000007FF);
        if (((ADDR2MUX_output & 0x00000400) >> 10) == 1)
            ADDR2MUX_output = ADDR2MUX_output | 0xFFFFF800;
    }
    if(LSHF1 == 1)
        ADDR2MUX_output = ADDR2MUX_output << 1;

    ADDER_output = ADDR1MUX_output + ADDR2MUX_output;

    if (MARMUX == 0) // lshf
        MARMUX_output = (IR & 0x000000FF) << 1; 
    else if (MARMUX == 1) //adder
        MARMUX_output = ADDER_output;

    // PC
    PC_output = PC;
    /*
       if (PCMUX == 0)
       PC_output = PC + 2;
       else if (PCMUX == 1)
       PC_output = BUS;
       else if (PCMUX == 2)
       PC_output = ADDR1MUX_output + ADDR2MUX_output;
       */

    // ALU
    if (ALUK == 0)
        ALU_output = A + B;
    else if (ALUK == 1)
        ALU_output = A & B;
    else if (ALUK == 2)
        ALU_output = A ^ B;
    else if (ALUK == 3)
        ALU_output = A;

    // SHF
    if (IR4 == 0) //LSHF
        SHF_output = A << amount4;
    else if (IR5 == 0) //RSHFL
        SHF_output = A >> amount4;
    else if (IR5 == 1) // RSHFA
        SHF_output = Low16bits((A & 0x00008000) >> 15 == 0) ? (A >> amount4) : ((A|0xFFFF0000) >> amount4);

    // MDR
    int MDR_1;
    if (DATA_SIZE == 0) //BYTE
    {
        if (MAR0 == 0)
            MDR_1 = MDR & 0x000000FF;
        else
            MDR_1 = (MDR & 0x0000FF00) >> 8;
        
        if(MDR_1 >> 7 == 0) //positive
            MDR_output = MDR_1;
        else if (MDR_1 >> 7 == 1) //negative
            MDR_output = (MDR_1 | 0xFFFFFF00);
    }
    else
        MDR_output = MDR;

    if(print_show)
    {
        printf("\neval_bus_driver:\n");
        printf("MARMUX_output: %X\n", MARMUX_output);
        printf("PC_output: %X\n", PC_output);
        printf("ADDR1MUX_output: %X\n", ADDR1MUX_output);
        printf("ADDR2MUX_output: %X\n", ADDR2MUX_output);
        printf("A: %X\n", A);
        printf("B: %X\n", B);
        printf("ALU_output: %X\n", ALU_output);
        printf("SHF_output: %X\n", SHF_output);
        printf("MDR_output: %X\n", MDR_output);
    }
}

void drive_bus() {

    /* 
       Datapath routine for driving the bus from one of the 5 possible 
       tristate drivers. 
       */  
    int GATE_MARMUX = GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int GATE_PC = GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION);
    int GATE_ALU = GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION);
    int GATE_SHF = GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION);
    int GATE_MDR = GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION);

    BUS = 0;
    if (GATE_MARMUX == 1)
        BUS = Low16bits(MARMUX_output);
    else if(GATE_PC == 1)
        BUS = Low16bits(PC_output);
    else if(GATE_ALU == 1)
        BUS = Low16bits(ALU_output);
    else if(GATE_SHF == 1)
        BUS = Low16bits(SHF_output);
    else if(GATE_MDR == 1)
        BUS = Low16bits(MDR_output);

    if(print_show)
    {
        printf("\ndrive_bus:\n");
        printf("MARMUX_output: %X\n", MARMUX_output);
        printf("PC_output: %X\n", PC_output);
        printf("ALU_output: %X\n", ALU_output);
        printf("SHF_output: %X\n", SHF_output);
        printf("MDR_output: %X\n", MDR_output);
        printf("BUS: %X\n", BUS);    
    }
}


void latch_datapath_values() {

    /* 
       Datapath routine for computing all functions that need to latch
       values in the data path at the end of this cycle.  Some values
       require sourcing the bus; therefore, this routine has to come 
       after drive_bus.
       */       

    int LD_MAR = GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_MDR = GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_IR = GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_BEN = GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_REG = GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_CC = GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION);
    int LD_PC = GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION);
    int DRMUX = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int PCMUX = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);

    int MIO_EN = GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION);
    int DATA_SIZE = GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION);
    int MAR = CURRENT_LATCHES.MAR;
    int N = CURRENT_LATCHES.N;
    int Z = CURRENT_LATCHES.Z;
    int P = CURRENT_LATCHES.P;
    int IR11 = (CURRENT_LATCHES.IR & 0x0000800) >> 11;
    int IR10 = (CURRENT_LATCHES.IR & 0x0000400) >> 10;
    int IR9 = (CURRENT_LATCHES.IR & 0x0000200) >> 9;
    int DR = (DRMUX == 0) ? (CURRENT_LATCHES.IR & 0x00000E00) >> 9 : 7;

    if (LD_MAR == 1)
        NEXT_LATCHES.MAR = BUS;

    if (LD_MDR == 1)
    {
        if(MIO_EN == 0)
            if(DATA_SIZE == 0)
                NEXT_LATCHES.MDR = Low16bits((BUS & 0x000000FF) | BUS<<8);
            else
                NEXT_LATCHES.MDR = BUS;
        else if(MIO_EN == 1)
            if(CURRENT_LATCHES.READY == 1)
                NEXT_LATCHES.MDR = MEMORY[MAR >> 1][0] + (MEMORY[MAR >> 1][1] << 8);
            else
                NEXT_LATCHES.MDR = CURRENT_LATCHES.MDR;
        //else
        //    NEXT_LATCHES.MDR = MEMORY[MAR >> 1][0] + (MEMORY[MAR >> 1][1] << 8);
    }

    if (LD_IR == 1)
        NEXT_LATCHES.IR = BUS;

    if (LD_BEN == 1)
        NEXT_LATCHES.BEN = (IR11 && N) || (IR10 && Z) || (IR9 && P);

    if (LD_REG == 1)
        NEXT_LATCHES.REGS[DR] = BUS; 

    if (LD_CC == 1)
    {
        if (BUS == 0)
        {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 1;
            NEXT_LATCHES.P = 0;
        }
        else if (BUS >= (1<<15))
        {
            NEXT_LATCHES.N = 1;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 0;
        }
        else
        {
            NEXT_LATCHES.N = 0;
            NEXT_LATCHES.Z = 0;
            NEXT_LATCHES.P = 1;
        }
    }

    if (LD_PC == 1)
    {
        if (PCMUX == 0)
            NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
        else if (PCMUX == 1)
            NEXT_LATCHES.PC = BUS;
        else if (PCMUX == 2)
            NEXT_LATCHES.PC = ADDER_output;
    }


    if(print_show)
    {
        printf("\nlatch_datapath_values:\n");
        printf("NEXT_LATCHES.MAR: %X\n", NEXT_LATCHES.MAR);
        printf("NEXT_LATCHES.MDR: %X\n", NEXT_LATCHES.MDR);
        printf("NEXT_LATCHES.IR: %X\n", NEXT_LATCHES.IR);
        printf("NEXT_LATCHES.BEN: %X\n", NEXT_LATCHES.BEN);
        printf("DR: %d\n", DR);
        printf("NEXT_LATCHES.REGS[DR]: %X\n", NEXT_LATCHES.REGS[DR]);
        printf("NEXT_LATCHES.N: %X\n", NEXT_LATCHES.N);
        printf("NEXT_LATCHES.Z: %X\n", NEXT_LATCHES.Z);
        printf("NEXT_LATCHES.P: %X\n", NEXT_LATCHES.P);
        printf("NEXT_LATCHES.PC: %X\n", NEXT_LATCHES.PC);
    }
}

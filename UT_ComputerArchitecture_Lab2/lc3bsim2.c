/*
    Name 1: Ang Hsuan Lee
    UTEID 1: al48944
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

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
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
 * MEMORY[A][1] stores the most significant byte of word at word address A 
 * */

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;    /* run bit */


typedef struct System_Latches_Struct{

    int PC,       /* program counter */
        N,        /* n condition bit */
        Z,        /* z condition bit */
        P;        /* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
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
/* Purpose   : Simulate the LC-3b until HALTed                 */
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
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
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
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
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

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;  
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
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
                argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
 * You are allowed to use the following global variables in your
 * code. These are defined above.
 *
 * MEMORY
 *
 * CURRENT_LATCHES
 * NEXT_LATCHES
 *
 * You may define your own local/global variables and functions.
 * You may use the functions to get at the control bits defined
 * above.
 *
 * Begin your code here                     */

/***************************************************************/

// Convert 4-bit binary string to 1-bit hex string, ex: "1010" -> "A"
char bin2hex( char *bin)
{
    int dec = 0;
    for (int i = 0; i < 4; i++)
        dec += (*(bin+i)-48)*pow(2, 3-i);
    if (dec >= 0 && dec <= 9)
        return (dec + 48);
    else if(dec >= 10 && dec <= 16)
        return (dec + 55);
    else
        return 0;
}

// Convert 16-bit string into 4-bit hex string, ex: "0000 0001 0001 1111" -> "011F"
char* string2hex( char *str)
{
    static char str_hex[4];
    str_hex[0] = bin2hex(str);
    str_hex[1] = bin2hex(str+4);
    str_hex[2] = bin2hex(str+8);
    str_hex[3] = bin2hex(str+12);
    return str_hex;
}

// binary char* to decimal int
int bin2dec(char *bin, int sign)
{
    int dec = 0;
    int sign_neg = 0;
    int bit_ctr = 0;
    int neg_flag = (*bin) == '1'? 1 : 0;

    while(*bin != '\0')
    {
        dec = dec*2 + (*bin - 48);
        bin ++;
        bit_ctr ++;
    }
    if (sign == 1 & neg_flag == 1)
    {
        sign_neg = pow(2, bit_ctr);
        dec = dec - sign_neg;
    }
    return dec;
}

// Substring
char* substring(char *str, int pos, int len)
{
    char* substr = (char*)malloc((len+1) * sizeof(char));
    for (int i = 0; i < len; i++)
        memset(substr + i, str[pos+i], sizeof(char));
    memset(substr + len, '\0', sizeof(char));
    return substr;
}

// Sign Extension From 8 bits to 16 bits
int signExtension(int i) {
    int value = (0x00FF & i);
    int mask = 0x0080;
    if (mask & i) {
        value += 0xFF00;
    }
    return value;
}

// Sign Extension From 5 bits to 16 bits
int signExtensionNOT(int i) {
    int value = (0x001F & i);
    int mask = 0x0010;
    if (mask & i) {
        value += 0xFFE0;
    }
    return value;
}

// handle AND opcode
void handleAND(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *sr1 = substring(binInst, 7, 3);
    char *bit5 = substring(binInst, 10, 1);
    int data;
    if(*bit5 == '0')
    {
        char *sr2 = substring(binInst, 13, 3);
        int SR2 = bin2dec(sr2, 0);
        data = CURRENT_LATCHES.REGS[SR2];
        free(sr2);
    }
    else if(*bit5 == '1')
    {
        char *imm5 = substring(binInst, 11, 5);
        data = bin2dec(imm5, 1);
        free(imm5);
    }
    int DR = bin2dec(dr, 0);
    int SR1 = bin2dec(sr1, 0);
    free(dr);
    free(sr1);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    data = Low16bits(CURRENT_LATCHES.REGS[SR1]) & Low16bits(data);
    NEXT_LATCHES.REGS[DR] = data;

    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    if(data == 0)
        NEXT_LATCHES.Z = 1;
    else if((data>>15) == 0)
        NEXT_LATCHES.P = 1;
    else
        NEXT_LATCHES.N = 1;
}

// handle XOR opcode
void handleXOR(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *sr1 = substring(binInst, 7, 3);
    char *bit5 = substring(binInst, 10, 1);
    int data;
    if(*bit5 == '0')
    {
        char *sr2 = substring(binInst, 13, 3);
        int SR2 = bin2dec(sr2, 0);
        data = CURRENT_LATCHES.REGS[SR2];
        data = data;
        free(sr2);
    }
    else if(*bit5 == '1')
    {
        char *imm5 = substring(binInst, 11, 5);
        data = bin2dec(imm5, 0);
        data = signExtensionNOT(data);
        free(imm5);
    }
    int DR = bin2dec(dr, 0);
    int SR1 = bin2dec(sr1, 0);
    free(dr);
    free(sr1);

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    data = Low16bits(CURRENT_LATCHES.REGS[SR1]) ^ Low16bits(data);
    NEXT_LATCHES.REGS[DR] = data;

    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    if(data == 0)
        NEXT_LATCHES.Z = 1;
    else if((data>>15) == 0)
        NEXT_LATCHES.P = 1;
    else
        NEXT_LATCHES.N = 1;
}

// handle ADD opcode
void handleADD(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *sr1 = substring(binInst, 7, 3);
    char *bit5 = substring(binInst, 10, 1);
    int data;
    if(*bit5 == '0')
    {
        char *sr2 = substring(binInst, 13, 3);
        int SR2 = bin2dec(sr2, 0);
        data = CURRENT_LATCHES.REGS[SR2];
        free(sr2);
    }
    else if(*bit5 == '1')
    {
        char *imm5 = substring(binInst, 11, 5);
        data = bin2dec(imm5, 1);
        free(imm5);
    }
    int DR = bin2dec(dr, 0);
    int SR1 = bin2dec(sr1, 0);
    free(dr);
    free(sr1);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    data = Low16bits(CURRENT_LATCHES.REGS[SR1] + data);
    NEXT_LATCHES.REGS[DR] = data;

    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    if(data == 0)
        NEXT_LATCHES.Z = 1;
    else if((data>>15) == 0)
        NEXT_LATCHES.P = 1;
    else
        NEXT_LATCHES.N = 1;
}

// handle LEA opcode
void handleLEA(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *pc9 = substring(binInst, 7, 9);
    int DR = bin2dec(dr, 0);
    int PCoffset9 = bin2dec(pc9, 1);
    free(dr);
    free(pc9);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    NEXT_LATCHES.REGS[DR] = Low16bits(NEXT_LATCHES.PC + 2*PCoffset9);
    //LEA does NOT set cc
    /*
       NEXT_LATCHES.N = 0;
       NEXT_LATCHES.Z = 0;
       NEXT_LATCHES.P = 0;
       if(PCoffset9 == 0)
       NEXT_LATCHES.Z = 1;
       else if(PCoffset9 > 0)
       NEXT_LATCHES.P = 1;
       else
       NEXT_LATCHES.N = 1;
       */        
}

// handle LDW opcode
void handleLDW(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *br = substring(binInst, 7, 3);
    char *of6 = substring(binInst, 10, 6);
    int DR = bin2dec(dr, 0);
    int BaseR = bin2dec(br, 0);
    int offset6 = bin2dec(of6, 1);
    free(dr);
    free(br);
    free(of6);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    int data = Low16bits((MEMORY[Low16bits((CURRENT_LATCHES.REGS[BaseR] + 2*offset6)) >> 1][1] << 8) +
        MEMORY[Low16bits((CURRENT_LATCHES.REGS[BaseR] + 2*offset6)) >> 1][0]);
    NEXT_LATCHES.REGS[DR] = data;

    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    if(data == 0)
        NEXT_LATCHES.Z = 1;
    else if((data>>15) == 0)
        NEXT_LATCHES.P = 1;
    else
        NEXT_LATCHES.N = 1;
}

// handle STW opcode
void handleSTW(char *binInst)
{
    char *sr = substring(binInst, 4, 3);
    char *br = substring(binInst, 7, 3);
    char *of6 = substring(binInst, 10, 6);
    int SR = bin2dec(sr, 0);
    int BaseR = bin2dec(br, 0);
    int offset6 = bin2dec(of6, 1);
    free(sr);
    free(br);
    free(of6);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    int data = CURRENT_LATCHES.REGS[SR];
    MEMORY[Low16bits((CURRENT_LATCHES.REGS[BaseR] + 2*offset6)) >> 1][0] = data & 0x00FF;
    MEMORY[Low16bits((CURRENT_LATCHES.REGS[BaseR] + 2*offset6)) >> 1][1] = (data >> 8) & 0x00FF;
}

// handle LDB opcode
void handleLDB(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *br = substring(binInst, 7, 3);
    char *bof6 = substring(binInst, 10, 6);
    int DR = bin2dec(dr, 0);
    int BaseR = bin2dec(br, 0);
    int boffset6 = bin2dec(bof6, 1);
    free(dr);
    free(br);
    free(bof6);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    int new_addr = Low16bits(CURRENT_LATCHES.REGS[BaseR] + boffset6);
    int index = new_addr%2;
    int data = signExtension(MEMORY[new_addr >> 1][index]);
    NEXT_LATCHES.REGS[DR] = Low16bits(data);

    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    if(data == 0)
        NEXT_LATCHES.Z = 1;
    else if((data>>15) == 0)
        NEXT_LATCHES.P = 1;
    else
        NEXT_LATCHES.N = 1;
}

// handle STB opcode
void handleSTB(char *binInst)
{
    char *sr = substring(binInst, 4, 3);
    char *br = substring(binInst, 7, 3);
    char *bof6 = substring(binInst, 10, 6);
    int SR = bin2dec(sr, 0);
    int BaseR = bin2dec(br, 0);
    int boffset6 = bin2dec(bof6, 1);
    free(sr);
    free(br);
    free(bof6);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    int new_addr = Low16bits(CURRENT_LATCHES.REGS[BaseR] + boffset6);
    int index = new_addr%2;
    int data = CURRENT_LATCHES.REGS[SR];
    MEMORY[new_addr >> 1][index] = data & 0x00FF;
}

// handle SHF opcode
void handleSHF(char *binInst)
{
    char *dr = substring(binInst, 4, 3);
    char *sr = substring(binInst, 7, 3);
    char *bit5 = substring(binInst, 10, 1);
    char *bit4 = substring(binInst, 11, 1);
    char *am4 = substring(binInst, 12, 4);

    int DR = bin2dec(dr, 0);
    int SR = bin2dec(sr, 0);
    int amount4 = bin2dec(am4, 0);
    free(dr);
    free(sr);
    free(am4);

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
    int data;
    if (*bit5 == '0' & *bit4 == '0')
        data = Low16bits(CURRENT_LATCHES.REGS[SR] << amount4);
    else if (*bit5 == '0' & *bit4 == '1')
        data = Low16bits(CURRENT_LATCHES.REGS[SR] >> amount4);
    else if (*bit5 == '1' & *bit4 == '1' & (CURRENT_LATCHES.REGS[SR]>>15 == 0))
        data = Low16bits(CURRENT_LATCHES.REGS[SR] >> amount4);
    else if (*bit5 == '1' & *bit4 == '1' & (CURRENT_LATCHES.REGS[SR]>>15 == 1))
        data = Low16bits((CURRENT_LATCHES.REGS[SR]|0xFFFF0000) >> amount4);

    NEXT_LATCHES.REGS[DR] = data;
    free(bit5);
    free(bit4);

    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
    if(data == 0)
        NEXT_LATCHES.Z = 1;
    else if((data>>15) == 0)
        NEXT_LATCHES.P = 1;
    else
        NEXT_LATCHES.N = 1;
}

// handle BR opcode
void handleBR(char *binInst)
{
    char *n = substring(binInst, 4, 1);
    char *z = substring(binInst, 5, 1);
    char *p = substring(binInst, 6, 1);
    char *pc9 = substring(binInst, 7, 9);

    int N = bin2dec(n, 0);
    int Z = bin2dec(z, 0);
    int P = bin2dec(p, 0);
    int PCoffset9 = bin2dec(pc9, 1);
    free(n);
    free(z);
    free(p);
    free(pc9);

    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
    if ((N == 1 & CURRENT_LATCHES.N == 1) |
            (Z == 1 & CURRENT_LATCHES.Z == 1) |
            (P == 1 & CURRENT_LATCHES.P == 1))
        NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (PCoffset9 << 1));
}

// handle JSR opcode
void handleJSR(char *binInst)
{
    char *bit11 = substring(binInst, 4, 1);
    int data;
    if(*bit11 == '0')                           //JSRR
    {
        char *br = substring(binInst, 7, 3);
        int baseR = bin2dec(br, 0);
        data = CURRENT_LATCHES.REGS[baseR];
        free(br);
    }
    else if(*bit11 == '1')                      //JSR
    {
        char *of11 = substring(binInst, 5, 11);
        data = bin2dec(of11, 1);
        free(of11);
    }

    if(*bit11 == '0')                           //JSRR
        NEXT_LATCHES.PC = Low16bits(data);
    else if(*bit11 == '1')                      //JSR
        NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2 + (data << 1));
    NEXT_LATCHES.REGS[7] = Low16bits(CURRENT_LATCHES.PC + 2);

    free(bit11);
}

// handle JMP opcode
void handleJMP(char *binInst)
{
    char *br = substring(binInst, 7, 3);
    int baseR = bin2dec(br, 0);
    free(br);

    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
}

// handle TRAP opcode
void handleTRAP(char *binInst)
{
    char *tv8 = substring(binInst, 8, 8);

    int trapvector8 = bin2dec(tv8, 0);
    free(tv8);

    NEXT_LATCHES.REGS[7] = Low16bits(CURRENT_LATCHES.PC + 2);
    NEXT_LATCHES.PC = Low16bits((MEMORY[Low16bits(trapvector8)][1] << 8) + MEMORY[Low16bits(trapvector8)][0]);
}

void process_instruction(){
    /*  function: process_instruction
     *  *  
     *  *    Process one instruction at a time  
     *  *       -Fetch one instruction
     *  *       -Decode 
     *  *       -Execute
     *  *       -Update NEXT_LATCHES
     *  */     

    int addr = CURRENT_LATCHES.PC >> 1;
    int print = 0;

    char binInst[16];
    for(int i = 0; i < 8; i++)
    {
        int data0 = MEMORY[addr][0];
        int data1 = MEMORY[addr][1];

        binInst[7-i] = (data1 >> i) % 2 + 48;
        binInst[15-i] = (data0 >> i) % 2 + 48;
    }
    // binInst[16] = '\0';
    char *hexInst = string2hex(binInst);
    //printf("%c\n", hexInst[0]);

    switch(hexInst[0])
    {
        case '0':                                   // BR
            if(print == 1) printf("Now proceeding BR...: %s\n", hexInst); 
            handleBR(binInst);
            break;
        case '1':                                   // ADD
            if(print == 1) printf("Now proceeding ADD...: %s\n", hexInst); 
            handleADD(binInst);
            break;
        case '2':                                   // LDB
            if(print == 1) printf("Now proceeding LDB...: %s\n", hexInst); 
            handleLDB(binInst);
            break;
        case '3':                                   // STB
            if(print == 1) printf("Now proceeding STB...: %s\n", hexInst); 
            handleSTB(binInst);
            break;
        case '4':                                   //JSR/JSRR
            if(print == 1) printf("Now proceeding JSR...: %s\n", hexInst); 
            handleJSR(binInst);
            break;
        case '5':                                   // AND
            if(print == 1) printf("Now proceeding AND...: %s\n", hexInst); 
            handleAND(binInst);
            break;
        case '6':                                   // LDW
            if(print == 1) printf("Now proceeding LDW...: %s\n", hexInst); 
            handleLDW(binInst);
            break;
        case '7':                                   // STW
            if(print == 1) printf("Now proceeding STW...: %s\n", hexInst); 
            handleSTW(binInst);
            break;
        case '8':                                   // RTI
            if(print == 1) printf("RTI (Do not need to handle..)"); 
            break;
        case '9':                                   // XOR/NOT
            if(print == 1) printf("Now proceeding XOR...: %s\n", hexInst); 
            handleXOR(binInst);
            break;
        case 'A':                                   // NAN
            if(print == 1) printf("A"); 
            break;
        case 'B':                                   // NAN
            if(print == 1) printf("B"); 
            break;
        case 'C':                                   // JMP/RET
            if(print == 1) printf("Now proceeding JMP...: %s\n", hexInst); 
            handleJMP(binInst);
            break;
        case 'D':                                   // SHF 
            if(print == 1) printf("Now proceeding SHF...: %s\n", hexInst); 
            handleSHF(binInst);
            break;
        case 'E':                                   // LEA 
            if(print == 1) printf("Now proceeding LEA...: %s\n", hexInst); 
            handleLEA(binInst);
            break;
        case 'F':                                   // TRAP 
            if(print == 1) printf("Now proceeding TRAP...: %s\n", hexInst); 
            handleTRAP(binInst);
            break;
        default: 
            if(print == 1) printf("default"); 
            break;
    }
}

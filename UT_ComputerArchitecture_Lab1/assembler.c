/*
   Name 1: Ang Hsuan Lee
   Name 2: Hsiao Yuan Chen
   UTEID 1: al48944
   UTEID 2: hc27746
*/
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#include <math.h>

#define MAX_LINE_LENGTH 255
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255

FILE* infile = NULL;
FILE* outfile = NULL;
const int err_print = 1;

typedef struct {
    int address;
    int offset;
    char label[MAX_LABEL_LEN + 1];  /* Question for the reader: Why do we need to add 1? */
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

const char * PseudoOpCodeList[] = {".orig", ".fill", ".end"};
const char * OpCodeList[] = {"add", "and", "br", "brn", "brp", "brz", "brnp", "brnz", "brpz", "brnzp",
    "halt", "jmp", "jsr", "jsrr", "ldb", "ldw", "lea", "nop", "not",
    "ret", "lshf", "rshfl", "rshfa", "rti", "stb", "stw", "trap", "xor"};
const char * RegList[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
const char * TrapCommandList[] = {"in", "out", "getc", "puts"};

enum
{
    DONE, OK, EMPTY_LINE
};

// Determine if a string is PseudoOpCode
int isPseudoOpcode(char *lPtr)
{
    int len = sizeof(PseudoOpCodeList)/sizeof(PseudoOpCodeList[0]);

    for (int i = 0; i < len; i++)
        if (!strcmp(lPtr, PseudoOpCodeList[i]))
            return 1;
    return -1;
}

// Determine if a string is OpCode
int isOpcode(char *lPtr)
{
    int len = sizeof(OpCodeList)/sizeof(OpCodeList[0]);

    for (int i = 0; i < len; i++)
        if (!strcmp(lPtr, OpCodeList[i]))
            return 1;
    return -1;
}

// Determine if a string is valid register
int isRegister(char *reg)
{
    int len = sizeof(RegList)/sizeof(RegList[0]);

    for (int i = 0; i < len; i++)
        if (!strcmp(reg, RegList[i]))
            return 1;
    return -1;
}

// Determine if a string is trap command
int isTrapCommand(char *lPtr)
{
    int len = sizeof(TrapCommandList)/sizeof(TrapCommandList[0]);

    for (int i = 0; i < len; i++)
        if (!strcmp(lPtr, TrapCommandList[i]))
            return 1;
    return -1;
}

// Convert string to decimal number
int toNum( char * pStr )
{
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )             /* decimal */
    { 
        pStr++;
        if( *pStr == '-' )                /* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr))
            {
                if (err_print) printf("Error: invalid decimal operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
            lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' ) /* hex     */
    {
        pStr++;
        if( *pStr == '-' )             /* hex is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))
            {
                if (err_print) printf("Error: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
            lNum = -lNum;
        return lNum;
    }
    else
    {
        if (err_print) printf( "Error code 4: invalid operand, %s\n", orig_pStr);
        exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
}

// Read FILE string and parse it into different arguments
int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
        ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );

    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
            *lPtr != '\n' ) 
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) ) 
        return( EMPTY_LINE );

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }

    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg1 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}

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

// Convert decimal number to size-bit binary string, ex: dec2bin(1, 3) -> "001"
int* dec2bin(int dec, int size)
{
    int* ptr = (int*)malloc(size * sizeof(int)); 
    for (int i = size-1; i >= 0; i--)
        memset(ptr + i, (dec >> i) & 1, sizeof(int)); 
    return ptr;
}

// Convert register name to 3-bit binary string, ex: "r1" -> "001"
int* reg2isa(char *reg)
{
    int dec_reg = (reg[1] - '0');
    return dec2bin(dec_reg, 3);
}

// Handle .ORIG opcode
int handleOrig(char *lOpcode, char *lArg1, char *lArg2, int lineCounter)
{
    // wrong number of operands
    if ( strcmp(lArg2, ""))
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg2);
        exit(4);
    }
    if ( !strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: missing operands (Orig Address)\n", lineCounter);
        exit(4);
    }

    // lArg1, invalid address, incorrect number will be handled in toNum function
    int addr = toNum(lArg1);
    if ( addr % 2 == 1)
    {
        if (err_print) printf("Error code 3 at line %d: invalid odd address\n", lineCounter);
        exit(3);
    }
    //else if( addr < 0 )
    //{
    //    printf("Error code 3 at line %d: invalid negative address\n", lineCounter);
    //    exit(3);
    //}

    // invalid constant
    if( addr > (int)pow(2, 16)-1 || addr < 0)
    {
        if (err_print) printf("Error code 3 at line %d: %d is an invalid constant (ORIG address range: 0 ~ 2^16-1)\n", lineCounter, addr);
        exit(3);
    }
    fprintf( outfile, "0x%04X\n", addr);
    return addr;
}

// Handle .FILL opcode
void handleFill(char *lOpcode, char *lArg1, char *lArg2, int lineCounter)
{
    // wrong number of operands
    if ( strcmp(lArg2, ""))
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg2);
        exit(4);
    }
    if ( !strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: missing operands (Fill_in_16bits_Num)\n", lineCounter);
        exit(4);
    }

    // invalid constant
    int fillNum = toNum(lArg1);
    if( fillNum > (int)(pow(2, 16)-1) || fillNum < (int)(-pow(2, 15)))
    {
        if (err_print) printf("Error code 3 at line %d: %d is an invalid constant (offset range: -2^15 ~ 2^16-1)\n", lineCounter, fillNum);
        exit(3);
    }
    // negative hex representation
    if (fillNum < 0)
        fillNum = 65536 + fillNum;

    fprintf( outfile, "0x%04X\n", fillNum);
}

// Handle .END opcode
void handleEnd(char *lOpcode, char *lArg1, int lineCounter)
{
    // wrong number of operands
    if ( strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg1);
        exit(4);
    }
}

// Handle ADD type opcode: ADD AND XOR NOT
void handleAddType(char *lOpcode, char *lArg1, char *lArg2, char *lArg3, char *lArg4, int lineCounter)
{
    char ISA[16];

    // Opcode
    if (!strcmp(lOpcode, "add"))
    {
        ISA[0] = '0'; ISA[1] = '0';
    }
    else if (!strcmp(lOpcode, "and"))
    {
        ISA[0] = '0'; ISA[1] = '1';
    }
    else if (!strcmp(lOpcode, "xor") || !strcmp(lOpcode, "not"))
    {
        ISA[0] = '1'; ISA[1] = '0';
    }
    ISA[2] = '0'; ISA[3] = '1';

    // Operands
    // wrong number of operands
    if (!strcmp(lOpcode, "not"))
    {
        if ( strcmp(lArg3, ""))
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg3);
            exit(4);
        }
        if ( !strcmp(lArg2, "") || !strcmp(lArg1, ""))
        {
            if (err_print) printf("Error code 4 at line %d: missing operands (Register)\n", lineCounter);
            exit(4);
        }
        // Function compatibility
        lArg3 = "#-1";
    }
    else
    {
        if ( strcmp(lArg4, ""))
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg4);
            exit(4);
        }
        if ( !strcmp(lArg2, "") || !strcmp(lArg3, "") || !strcmp(lArg1, ""))
        {
            if (err_print) printf("Error code 4 at line %d: missing operands (Register or Imm5)\n", lineCounter);
            exit(4);
        }
    }

    if( isRegister(lArg1) == 1 && isRegister(lArg2) == 1)
    {
        int* DR = reg2isa(lArg1);
        ISA[4] = 48 + DR[2]; ISA[5] = 48 + DR[1]; ISA[6] = 48 + DR[0];
        free(DR);

        int* SR1 = reg2isa(lArg2);
        ISA[7] = 48 + SR1[2]; ISA[8] = 48 + SR1[1]; ISA[9] = 48 + SR1[0];
        free(SR1);

        if (isRegister(lArg3) == 1)
        {
            int *SR2 = reg2isa(lArg3);
            ISA[10] = '0'; ISA[11] = '0'; ISA[12] = '0';
            ISA[13] = 48 + SR2[2]; ISA[14] = 48 + SR2[1]; ISA[15] = 48 + SR2[0];
            free(SR2);    
        }
        else
        {            
            ISA[10] = '1';
            // invalid constant
            if( toNum(lArg3) > 15 || toNum(lArg3) < -16)
            {
                if (err_print) printf("Error code 3 at line %d: %d is an invalid constant (Imm5 range: -2^4 ~ 2^4-1)\n", lineCounter, toNum(lArg3));
                exit(3);
            }
            // incorrect number will be handled in toNum function
            int *imm5 = dec2bin(toNum(lArg3), 5);
            ISA[11] = 48 + imm5[4]; ISA[12] = 48 + imm5[3];
            ISA[13] = 48 + imm5[2]; ISA[14] = 48 + imm5[1]; ISA[15] = 48 + imm5[0];
            free(imm5);
        }
    }
    // unexpected operands in R1, R2
    else if (isRegister(lArg1) == -1)
    {
        if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg1);
        exit(4);
    }
    else if (isRegister(lArg2) == -1)
    {
        if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg2);
        exit(4);
    }

    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle LEA opcode : LEA LABEL
void handleLeaType(char *lOpcode, char *lArg1, char *lArg2, char *lArg3, int labelNum, int PCnextCounter, int lineCounter)
{
    char ISA[16];
    int labelAvail = 0; //detect label is available or not

    // Opcode
    ISA[0] = '1'; ISA[1] = '1'; ISA[2] = '1'; ISA[3] = '0';

    // Operands
    // wrong number of operands
    if ( strcmp(lArg3, ""))
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg3);
        exit(4);
    }
    if ( !strcmp(lArg2, "") || !strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: missing operands (Label)\n", lineCounter);
        exit(4);
    }

    if( isRegister(lArg1) == 1)
    {
        int *DR = reg2isa(lArg1);
        ISA[4] = 48 + DR[2]; ISA[5] = 48 + DR[1]; ISA[6] = 48 + DR[0];
        free(DR);

        for( int i = 0; i < labelNum; i++)
        {
            if( !strcmp(lArg2, symbolTable[i].label))
            {
                labelAvail = 1;
                // invalid constant
                int offset = symbolTable[i].offset - PCnextCounter;
                if( offset > (int)(pow(2, 8)-1) || offset < (int)(-pow(2, 8)))
                {
                    if (err_print) printf("Error code 4 at line %d: %d is an invalid constant (Offset9 range: -2^8 ~ 2^8-1)\n", lineCounter, offset);
                    exit(4);
                }
                int *PCOffset9 = dec2bin(offset, 9);
                ISA[7] = 48 + PCOffset9[8]; ISA[8] = 48 + PCOffset9[7]; ISA[9] = 48 + PCOffset9[6];
                ISA[10] = 48 + PCOffset9[5]; ISA[11] = 48 + PCOffset9[4]; ISA[12] = 48 + PCOffset9[3];
                ISA[13] = 48 + PCOffset9[2]; ISA[14] = 48 + PCOffset9[1]; ISA[15] = 48 + PCOffset9[0];
                free(PCOffset9);
            }
        }
        if (labelAvail == 0)
        {
            if (isRegister(lArg2) == 1)
            {
                if (err_print) printf("Error code 4 at line %d: register %s is not an label\n", lineCounter, lArg2);
                exit(4);
            }
            if ((*lArg2) == 'x' || (*lArg2) == '#')
            {
                int isNum = 1;
                for (int i = 1; *(lArg2+i) != '\0'; i++)
                {
                    if( *(lArg2+i) > '9' || *(lArg2+i) < '0')
                    {
                        isNum = 0;
                        break;
                    }
                }
                if (isNum == 1)
                {
                    if (err_print) printf("Error code 4 at line %d: register %s is not an label\n", lineCounter, lArg2);
                    exit(4);
                }
            }
            if (err_print) printf("Error code 1 at line %d: %s is undefined label\n", lineCounter, lArg2);
            exit(1);
        }
    }
    // unexpected operands in R1
    else
    {
        if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg1);
        exit(4);
    }

    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle LDW type opcode: LDW LDB STW STB JMP JSRR RET
void handleLdwType(char *lOpcode, char *lArg1, char *lArg2, char *lArg3, char *lArg4, int labelNum, int PCnextCounter, int lineCounter)
{
    char ISA[16];
    int labelAvail = 0;

    // Opcode
    if (!strcmp(lOpcode, "ldw"))
    {
        ISA[0] = '0'; ISA[1] = '1'; ISA[2] = '1'; ISA[3] = '0';
    }
    else if (!strcmp(lOpcode, "ldb"))
    {
        ISA[0] = '0'; ISA[1] = '0'; ISA[2] = '1'; ISA[3] = '0';
    }
    else if (!strcmp(lOpcode, "stw"))
    {
        ISA[0] = '0'; ISA[1] = '1'; ISA[2] = '1'; ISA[3] = '1';
    }
    else if (!strcmp(lOpcode, "stb"))
    {
        ISA[0] = '0'; ISA[1] = '0'; ISA[2] = '1'; ISA[3] = '1';
    }
    else if (!strcmp(lOpcode, "jmp") || !strcmp(lOpcode, "ret"))
    {
        ISA[0] = '1'; ISA[1] = '1'; ISA[2] = '0'; ISA[3] = '0';
    }
    else if (!strcmp(lOpcode, "jsrr"))
    {
        ISA[0] = '0'; ISA[1] = '1'; ISA[2] = '0'; ISA[3] = '0';
    }

    // Operands
    // wrong number of operands
    if (!strcmp(lOpcode, "ldw") || !strcmp(lOpcode, "ldb") || !strcmp(lOpcode, "stw") || !strcmp(lOpcode, "stb") )
    {
        if ( strcmp(lArg4, ""))
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg4);
            exit(4);
        }
        if ( !strcmp(lArg2, "") || !strcmp(lArg1, "") || !strcmp(lArg3, "") )
        {
            if (err_print) printf("Error code 4 at line %d: missing operands (Register or Offset6)\n", lineCounter);
            exit(4);
        }
    }
    else if (!strcmp(lOpcode, "jmp") || !strcmp(lOpcode, "jsrr"))
    {
        if ( strcmp(lArg2, ""))
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg2);
            exit(4);
        }
        if ( !strcmp(lArg1, ""))
        {
            if (err_print) printf("Error code 4 at line %d: missing operands (Register)\n", lineCounter);
            exit(4);
        }
        // Function compatibility
        lArg2 = lArg1;
        lArg1 = "r0";
        lArg3 = "x0";
    }
    else if (!strcmp(lOpcode, "ret"))
    {
        if ( strcmp(lArg1, ""))
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg1);
            exit(4);
        }
        // Function compatibility
        lArg1 = "r0";
        lArg2 = "r7";
        lArg3 = "x0";
    }

    if( !strcmp(lOpcode, "ldw") ||!strcmp(lOpcode, "ldb") || !strcmp(lOpcode, "stw") || !strcmp(lOpcode, "stb"))
    {
        if( isRegister(lArg1) == 1 && isRegister(lArg2) == 1)
        {
            int *DR = reg2isa(lArg1);
            ISA[4] = 48 + DR[2]; ISA[5] = 48 + DR[1]; ISA[6] = 48 + DR[0];
            free(DR);

            int *BaseR = reg2isa(lArg2);
            ISA[7] = 48 + BaseR[2]; ISA[8] = 48 + BaseR[1]; ISA[9] = 48 + BaseR[0];
            free(BaseR);

            // invalid constant
            int offset = toNum(lArg3);
            if( offset > (int)(pow(2, 5)-1) || offset < (int)(-pow(2, 5)))
            {
                if (err_print) printf("Error code 3 at line %d: %d is an invalid constant (Offset9 range: -2^5 ~ 2^5-1)\n", lineCounter, offset);
                exit(3);
            }
            int *offset6 = dec2bin(offset, 6);
            ISA[10] = 48 + offset6[5]; ISA[11] = 48 + offset6[4]; ISA[12] = 48 + offset6[3];
            ISA[13] = 48 + offset6[2]; ISA[14] = 48 + offset6[1]; ISA[15] = 48 + offset6[0];
            free(offset6);
        }
        // unexpected operands in R1, R2
        else if (isRegister(lArg1) == -1)
        {
            if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg1);
            exit(4);
        }
        else if (isRegister(lArg2) == -1)
        {
            if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg2);
            exit(4);
        }
    }
    else if( !strcmp(lOpcode, "jmp") || !strcmp(lOpcode, "jsrr"))
    {
        if( isRegister(lArg2) == 1)
        {
            int *DR = reg2isa(lArg1);
            ISA[4] = 48 + DR[2]; ISA[5] = 48 + DR[1]; ISA[6] = 48 + DR[0];
            free(DR);

            int *BaseR = reg2isa(lArg2);
            ISA[7] = 48 + BaseR[2]; ISA[8] = 48 + BaseR[1]; ISA[9] = 48 + BaseR[0];
            free(BaseR);

            int *offset6 = dec2bin(toNum(lArg3), 6);
            ISA[10] = 48 + offset6[5]; ISA[11] = 48 + offset6[4]; ISA[12] = 48 + offset6[3];
            ISA[13] = 48 + offset6[2]; ISA[14] = 48 + offset6[1]; ISA[15] = 48 + offset6[0];
            free(offset6);
        }
        // unexpected operands in R1, R2
        else if (isRegister(lArg2) == -1)
        {
            if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg2);
            exit(4);
        }
    }
    else if( !strcmp(lOpcode, "ret"))
    {
        int *DR = reg2isa(lArg1);
        ISA[4] = 48 + DR[2]; ISA[5] = 48 + DR[1]; ISA[6] = 48 + DR[0];
        free(DR);

        int *BaseR = reg2isa(lArg2);
        ISA[7] = 48 + BaseR[2]; ISA[8] = 48 + BaseR[1]; ISA[9] = 48 + BaseR[0];
        free(BaseR);

        int *offset6 = dec2bin(toNum(lArg3), 6);
        ISA[10] = 48 + offset6[5]; ISA[11] = 48 + offset6[4]; ISA[12] = 48 + offset6[3];
        ISA[13] = 48 + offset6[2]; ISA[14] = 48 + offset6[1]; ISA[15] = 48 + offset6[0];
        free(offset6);
    }

    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle TRAP type opcode: TRAP HALT
void handleTrapType(char *lOpcode, char *lArg1, char *lArg2, int lineCounter)
{
    char ISA[16];

    // Opcode
    ISA[0] = '1'; ISA[1] = '1'; ISA[2] = '1'; ISA[3] = '1';

    //Operands
    // wrong number of operands
    if ( !strcmp(lOpcode, "trap"))
    {
        if ( strcmp(lArg2, "") )
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg2);
            exit(4);
        }
        else if ( !strcmp(lArg1, ""))
        {
            if (err_print) printf("Error code 4 at line %d: missing operands (Trapvector8)\n", lineCounter);
            exit(4);
        }
    }
    if ( !strcmp(lOpcode, "halt"))
    {
        if( strcmp(lArg1, "") )
        {
            if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg1);
            exit(4);
        }
        // Function compatibility
        lArg1 = "x25";
    }

    ISA[4] = '0'; ISA[5] = '0'; ISA[6] = '0'; ISA[7] = '0';

    // non-hex trapvector
    if( *lArg1 != 'x')
    {
        if (err_print) printf("Error code 3 at line %d: Trapvect8 %s should be a hex number\n", lineCounter, lArg1);
        exit(3);
    }
    // invalid constant !??
    int trapvect = toNum(lArg1);
    if( trapvect > (int)(pow(2, 8)-1) || trapvect < 0)
    {
        if (err_print) printf("Error code 3 at line %d: %d is an invalid constant (Trapvect8 range: 0 ~ 2^8-1)\n", lineCounter, trapvect);
        exit(3);
    }
    int *trapvect8 = dec2bin(trapvect, 8);
    ISA[8] = 48 + trapvect8[7]; ISA[9] = 48 + trapvect8[6]; ISA[10] = 48 + trapvect8[5]; ISA[11] = 48 + trapvect8[4]; 
    ISA[12] = 48 + trapvect8[3]; ISA[13] = 48 + trapvect8[2]; ISA[14] = 48 + trapvect8[1]; ISA[15] = 48 + trapvect8[0]; 
    free(trapvect8);

    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle BR type opcode: BR BRn BRz BRp BRnz BRnp BRzp BRnzp
void handleBrType(char *lOpcode, char *lArg1, char *lArg2, int labelNum, int PCnextCounter, int lineCounter)
{
    char ISA[16];
    int labelAvail = 0;

    // Opcode
    ISA[0] = '0'; ISA[1] = '0'; ISA[2] = '0'; ISA[3] = '0';

    // Operands
    if ( strcmp(lArg2, "") )
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg2);
        exit(4);
    }
    else if ( !strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: missing operands (Label)\n", lineCounter);
        exit(4);
    }

    if (!strcmp(lOpcode, "br") || (!strcmp(lOpcode, "brnzp")))
    {
        ISA[4] = '1'; ISA[5] = '1'; ISA[6] = '1';
    }
    else if (!strcmp(lOpcode, "brn"))
    {
        ISA[4] = '1'; ISA[5] = '0'; ISA[6] = '0';
    }
    else if (!strcmp(lOpcode, "brz"))
    {
        ISA[4] = '0'; ISA[5] = '1'; ISA[6] = '0';
    }
    else if (!strcmp(lOpcode, "brp"))
    {
        ISA[4] = '0'; ISA[5] = '0'; ISA[6] = '1';
    }
    else if (!strcmp(lOpcode, "brnz"))
    {
        ISA[4] = '1'; ISA[5] = '1'; ISA[6] = '0';
    }
    else if (!strcmp(lOpcode, "brnp"))
    {
        ISA[4] = '1'; ISA[5] = '0'; ISA[6] = '1';
    }
    else if (!strcmp(lOpcode, "brzp"))
    {
        ISA[4] = '0'; ISA[5] = '1'; ISA[6] = '1';
    }

    for( int i = 0; i < labelNum; i++)
    {
        if( !strcmp(lArg1, symbolTable[i].label))
        {
            labelAvail = 1;
            // invalid constant
            int offset = symbolTable[i].offset - PCnextCounter;
            if( offset > (int)(pow(2, 8)-1) || offset < (int)(-pow(2, 8)))
            {
                if (err_print) printf("Error code 4 at line %d: %d is an invalid constant (Offset9 range: -2^8 ~ 2^8-1)\n", lineCounter, offset);
                exit(4);
            }
            int *PCOffset9 = dec2bin(offset, 9);
            ISA[7] = 48 + PCOffset9[8]; ISA[8] = 48 + PCOffset9[7]; ISA[9] = 48 + PCOffset9[6];
            ISA[10] = 48 + PCOffset9[5]; ISA[11] = 48 + PCOffset9[4]; ISA[12] = 48 + PCOffset9[3];
            ISA[13] = 48 + PCOffset9[2]; ISA[14] = 48 + PCOffset9[1]; ISA[15] = 48 + PCOffset9[0];
            free(PCOffset9);
        }
    }
    if (labelAvail == 0)
    {
        if (isRegister(lArg1) == 1)
        {
            if (err_print) printf("Error code 4 at line %d: register %s is not an label\n", lineCounter, lArg1);
            exit(4);
        }
        if ((*lArg1) == 'x' || (*lArg1) == '#')
        {
            int isNum = 1;
            for (int i = 1; *(lArg1+i) != '\0'; i++)
            {
                if( *(lArg1+i) > '9' || *(lArg1+i) < '0')
                {
                    isNum = 0;
                    break;
                }
            }
            if (isNum == 1)
            {
                if (err_print) printf("Error code 4 at line %d: register %s is not an label\n", lineCounter, lArg1);
                exit(4);
            }
        }
        if (err_print) printf("Error code 1 at line %d: %s is undefined label\n", lineCounter, lArg1);
        exit(1);
    }

    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle SHF type opcode: LSHF RSHFL RSHFA
void handleShfType(char *lOpcode, char *lArg1, char *lArg2, char *lArg3, char *lArg4, int lineCounter)
{
    char ISA[16];

    // Opcode
    ISA[0] = '1'; ISA[1] = '1'; ISA[2] = '0'; ISA[3] = '1';

    // Operands
    // wrong number of operands
    if ( strcmp(lArg4, ""))
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg4);
        exit(4);
    }
    if ( !strcmp(lArg2, "") || !strcmp(lArg3, "") || !strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: missing operands (Register or Amount4)\n", lineCounter);
        exit(4);
    }

    if( isRegister(lArg1) == 1 && isRegister(lArg2) == 1)
    {
        int *DR = reg2isa(lArg1);
        ISA[4] = 48 + DR[2]; ISA[5] = 48 + DR[1]; ISA[6] = 48 + DR[0];
        free(DR);

        int *SR = reg2isa(lArg2);
        ISA[7] = 48 + SR[2]; ISA[8] = 48 + SR[1]; ISA[9] = 48 + SR[0];
        free(SR);

        if ( !strcmp(lOpcode, "lshf"))
        {
            ISA[10] = '0'; ISA[11] = '0';
        }
        else if ( !strcmp(lOpcode, "rshfl"))
        {
            ISA[10] = '0'; ISA[11] = '1';
        }
        else if ( !strcmp(lOpcode, "rshfa"))
        {
            ISA[10] = '1'; ISA[11] = '1';
        }

        // invalid constant
        if( toNum(lArg3) > 15 || toNum(lArg3) < 0)
        {
            if (err_print) printf("Error code 3 at line %d: %d is an invalid constant (Amount4 range: 0 - 15)\n", lineCounter, toNum(lArg3));
            exit(3);
        }
        // incorrect number will be handled in toNum function
        int *amount4 = dec2bin(toNum(lArg3), 4);
        ISA[12] = 48 + amount4[3]; ISA[13] = 48 + amount4[2]; ISA[14] = 48 + amount4[1]; ISA[15] = 48 + amount4[0];
        free(amount4);
    }
    // unexpected operands in R1, R2
    else if (isRegister(lArg1) == -1)
    {
        if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg1);
        exit(4);
    }
    else if (isRegister(lArg2) == -1)
    {
        if (err_print) printf("Error code 4 at line %d: %s is an unexpected operands\n", lineCounter, lArg2);
        exit(4);
    }
    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle JSR type opcode: JSR
void handleJsrType(char *lOpcode, char *lArg1, char *lArg2, int labelNum, int PCnextCounter, int lineCounter)
{
    char ISA[16];
    int labelAvail = 0;

    // Opcode
    ISA[0] = '0'; ISA[1] = '1'; ISA[2] = '0'; ISA[3] = '0'; ISA[4] = '1';

    // Operands
    if ( strcmp(lArg2, "") )
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg2);
        exit(4);
    }
    else if ( !strcmp(lArg1, ""))
    {
        if (err_print) printf("Error code 4 at line %d: missing operands (Label)\n", lineCounter);
        exit(4);
    }

    for( int i = 0; i < labelNum; i++)
    {
        if( !strcmp(lArg1, symbolTable[i].label))
        {
            labelAvail = 1;
            // invalid constant
            int offset = symbolTable[i].offset - PCnextCounter;
            if( offset > (int)(pow(2, 10)-1) || offset < (int)(-pow(2, 10)))
            {
                if (err_print) printf("Error code 4 at line %d: %d is an invalid constant (Offset11 range: -2^10 ~ 2^10-1)\n", lineCounter, offset);
                exit(4);
            }
            int *PCOffset11 = dec2bin(offset, 11);
            ISA[5] = 48 + PCOffset11[10]; ISA[6] = 48 + PCOffset11[9]; ISA[7] = 48 + PCOffset11[8]; ISA[8] = 48 + PCOffset11[7];
            ISA[9] = 48 + PCOffset11[6]; ISA[10] = 48 + PCOffset11[5]; ISA[11] = 48 + PCOffset11[4]; ISA[12] = 48 + PCOffset11[3];
            ISA[13] = 48 + PCOffset11[2]; ISA[14] = 48 + PCOffset11[1]; ISA[15] = 48 + PCOffset11[0];
            free(PCOffset11);
        }
    }
    if (labelAvail == 0)
    {
        if (isRegister(lArg1) == 1)
        {
            if (err_print) printf("Error code 4 at line %d: register %s is not an label\n", lineCounter, lArg1);
            exit(4);
        }
        if ((*lArg1) == 'x' || (*lArg1) == '#')
        {
            int isNum = 1;
            for (int i = 1; *(lArg1+i) != '\0'; i++)
            {
                if( *(lArg1+i) > '9' || *(lArg1+i) < '0')
                {
                    isNum = 0;
                    break;
                }
            }
            if (isNum == 1)
            {
                if (err_print) printf("Error code 4 at line %d: register %s is not an label\n", lineCounter, lArg1);
                exit(4);
            }
        }

        if (err_print) printf("Error code 1 at line %d: %s is undefined label\n", lineCounter, lArg1);
        exit(1);
    }

    fprintf( outfile, "0x%s\n", string2hex(ISA));
}

// Handle RTI type opcode: RTI NOP
void handleRtiType(char *lOpcode, char *lArg1, int lineCounter)
{
    if ( strcmp(lArg1, "") )
    {
        if (err_print) printf("Error code 4 at line %d: unnecessary operands %s\n", lineCounter, lArg1);
        exit(4);
    }

    if ( !strcmp(lOpcode, "rti"))
        fprintf( outfile, "0x%d\n", 8000);
    else if( !strcmp(lOpcode, "nop"))
        fprintf( outfile, "0x%04d\n", 0);
}

// Convert Assembly to ISA
char* asm2isa(FILE* infile, int labelNum)
{
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet;
    int origAddr;
    int PCnextCounter = 0;  // next PC counter
    int lineCounter = 1;    // for error checking

    do
    {
        lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE)
        {
            //printf("%-8s%-8s%-8s%-8s%-8s%-8s -> ", lLabel, lOpcode, lArg1, lArg2, lArg3, lArg4);
            // pseudo-op
            if ( !strcmp(lOpcode, ".orig" ))
            {
                origAddr = handleOrig(lOpcode, lArg1, lArg2, lineCounter);
                PCnextCounter = 0;                       // restart
            }
            else if ( !strcmp(lOpcode, ".fill" ))
                handleFill(lOpcode, lArg1, lArg2, lineCounter);
            else if ( !strcmp(lOpcode, ".end" ))
            {
                handleEnd(lOpcode, lArg1, lineCounter);
                break;
            }
            // Following Opcodes are classified by ISA type
            // ADD ADD XOR NOT opcode
            else if ( !strcmp(lOpcode, "add" ) || !strcmp(lOpcode, "and" ) || !strcmp(lOpcode, "xor" ) || !strcmp(lOpcode, "not"))
                handleAddType(lOpcode, lArg1, lArg2, lArg3, lArg4, lineCounter);
            // LEA opcode
            else if ( !strcmp(lOpcode, "lea" ))
                handleLeaType(lOpcode, lArg1, lArg2, lArg3, labelNum, PCnextCounter, lineCounter);
            // LDW LDB STW STB JMP JSRR RET opcode
            else if ( !strcmp(lOpcode, "ldw" ) || !strcmp(lOpcode, "ldb") || !strcmp(lOpcode, "stw") || !strcmp(lOpcode, "stb") ||
                    !strcmp(lOpcode, "jmp") || !strcmp(lOpcode, "jsrr") || !strcmp(lOpcode, "ret"))
                handleLdwType(lOpcode, lArg1, lArg2, lArg3, lArg4, labelNum, PCnextCounter, lineCounter);
            // TRAP HALT opcode
            else if ( !strcmp(lOpcode, "trap" ) || !strcmp(lOpcode, "halt"))
                handleTrapType(lOpcode, lArg1, lArg2, lineCounter);
            // BR*8 opcode
            else if ( !strcmp(lOpcode, "br" ) || !strcmp(lOpcode, "brn" ) || !strcmp(lOpcode, "brz" ) || !strcmp(lOpcode, "brp" ) ||
                    !strcmp(lOpcode, "brzp" ) || !strcmp(lOpcode, "brnp" ) || !strcmp(lOpcode, "brnz" ) || !strcmp(lOpcode, "brnzp" ))
                handleBrType(lOpcode, lArg1, lArg2, labelNum, PCnextCounter, lineCounter);
            // LSHF RSHFL RSHFA opcode
            else if ( !strcmp(lOpcode, "lshf" ) || !strcmp(lOpcode, "rshfl" ) || !strcmp(lOpcode, "rshfa" ))
                handleShfType(lOpcode, lArg1, lArg2, lArg3, lArg4, lineCounter);
            // JSR opcode
            else if ( !strcmp(lOpcode, "jsr"))
                handleJsrType(lOpcode, lArg1, lArg2, labelNum, PCnextCounter, lineCounter);
            // RTI NOP opcode
            else if ( !strcmp(lOpcode, "rti") || !strcmp(lOpcode, "nop"))
                handleRtiType(lOpcode, lArg1, lineCounter);
            // Had already done in findLabel part...
            else
            {
                if (err_print) printf("(ex: .IN .FILL x90; Label start w/ '.')\n");
                if (err_print) printf("Error code 2 at line %d: %s is invalid opcode\n", lineCounter, lOpcode);
                exit(2);
            }
            // Suppose there is no invalid inst yet...
            PCnextCounter ++;
        }
        lineCounter ++;
    } while(lRet != DONE);
}

// Find and store all labels
int findLabel(FILE* infile)
{
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int lRet;
    int origAddr;               // store original address
    int labelCounter = 0;       // counter for label
    int instCounter = 0;        // counter for instruction
    int flagOrig = 0;           // detect orig opcode
    int lineCounter = 1;        // for error checking
    int flagEnd = 0;            // detect end opcode
    int labelCharCounter = 0;   // counter for label characters

    do
    {
        lRet = readAndParse( infile, lLine, &lLabel, &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( lRet != DONE && lRet != EMPTY_LINE)
        {
            if( !strcmp(lOpcode, ".orig"))
            {
                origAddr = toNum(lArg1);
                flagOrig ++;
                instCounter = 0;
            }
            else if(flagOrig == 0)
            {
                if (err_print) printf("Error code 4 at line %d: .ORIG must be in the first line\n", lineCounter);
                exit(4);
            }

            if( strcmp(lLabel, "") && flagOrig != 0)    // Process the label after .ORIG pseudo op
            {
                // Handle Error 2 Invalid Opcode
                if( isOpcode(lOpcode) == -1 && isPseudoOpcode(lOpcode) == -1)
                {
                    if (err_print) printf("Error code 2 at line %d: %s is invalid opcode\n", lineCounter, lOpcode);
                    exit(2);
                }
                // Invalid label: Opcode, Pusedo Opcode, Trap Command  -> ??????
                // (Priority)
                // Invalid label: Register 0-7, Start w/ 'x', Start w/o letter, Consist w/ non-alnum char
                if( isRegister(lLabel) == 1 || *lLabel == 'x' || !isalpha(*lLabel))
                {
                    if (err_print) printf("Error code 4 at line %d: invalid label\n", lineCounter);
                    exit(4);
                }

                char *labelPtr = lLabel;
                do
                {
                    if( !isalnum(*labelPtr))
                    {               
                        if (err_print) printf("Error code 4 at line %d: invalid label\n", lineCounter);
                        exit(4);
                    }

                    // label length
                    if (labelCharCounter > MAX_LABEL_LEN-1 )
                    {   
                        if (err_print) printf("Error code 4 ad line %d: invalid length of label (no more than 20)\n", lineCounter);
                        exit(4);
                    }

                    labelPtr ++;
                    labelCharCounter ++;
                } while(*labelPtr != '\0');
                labelCharCounter = 0;

                // Repeat Label
                for( int i = 0; i < labelCounter; i++)
                {
                    if( !strcmp(lLabel, symbolTable[i].label))
                    {
                        if (err_print) printf("Error code 4 at line %d: label %s is repeated\n", lineCounter, lLabel);
                        exit(4);
                    }
                }

                strcpy( symbolTable[labelCounter].label, lLabel);
                symbolTable[labelCounter].address = origAddr + 2*(instCounter-1);
                symbolTable[labelCounter].offset = instCounter-1;
                labelCounter ++;
            }

            instCounter ++;

            // .END detection and neglect the following lines
            if( !strcmp(lOpcode, ".end"))
            {
                flagEnd  = 1;
                break;
            }
        }
        lineCounter ++;

    } while(lRet != DONE);

    // No ORIG or END Handling
    if(flagOrig == 0)
    {
        if (err_print) printf("(Should not allow to display)\n");
        if (err_print) printf("Error : no .ORIG in assembly code\n");
        exit(4);
    }

    if(flagEnd == 0)
    {
        if (err_print) printf("Error : no .END in assembly code\n");
        exit(4);
    }

    // print symbolTable contents
    //for(int i = 0; i < labelCounter; i++)
    //    printf("0x%x: %s\n", symbolTable[i].address, symbolTable[i].label);

    return labelCounter;
}

int main(int argc, char* argv[]) {

    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");

    int labelNum;

    if (!infile) {
        if (err_print) printf("Error: Cannot open file %s\n", argv[1]);
        exit(4);
    }
    if (!outfile) {
        if (err_print) printf("Error: Cannot open file %s\n", argv[2]);
        exit(4);
    }

    /* Do stuff with files */
    labelNum = findLabel(infile);
    rewind(infile);
    asm2isa(infile, labelNum);

    fclose(infile);
    fclose(outfile);
}

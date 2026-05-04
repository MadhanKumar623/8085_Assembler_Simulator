/**********************************8085 SIMULATOR************************************/

/*  NOTE_1: correctly update the flags for each instructions
    NOTE_2: If a register pair is storing the address means (e.g. B and C) MSB is stored in B and LSB is stored in C
                      e.g if address is 800AH means B = 80, C = 0A
    NOTE_3: 8085 simulator has 16 bit of address bus so memory can be indicated by 2 bytes
    NOTE_4: Enter all the assembly code in UPPERCASE only, also don't leave space unnecessarily
    NOTE_5: Follow the LITTLE ENDIANESS while storing in memory
    NOTE_6: Use address as label during CALL and JUMP instructions
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

#define MEMORY_SIZE 65536
#define MAX_LINES 1000
#define MAX_LINE_LENGTH 128
#define MAX_LABELS 300

char memory[MEMORY_SIZE][15];                            //storing the memory contents,each of one byte
int addr;                                                            //index value for each memory location,starting address of machine code address
unsigned int A=0x00,B=0x00,C=0x00,D=0x00,E=0x00,H=0xAB,L=0x12,SP=0x5000;          //registers present in the 8085, stack pointer at 0x5000
unsigned int ZF,SF,PF,CF,AC;                             //initially all flag bits are get reset to 0
unsigned int flagRegister;                                         //containing all flag bits
unsigned int PC;                                                           //program counter

char str[MAX_LINES][MAX_LINE_LENGTH],f=0;
int size[MAX_LINES];
int len,len1;
unsigned int program_start_addr;
unsigned int program_end_addr;
char ports[256][15];

typedef struct {
    char label[MAX_LINE_LENGTH];
    unsigned int address;
} Symbol;

Symbol symbolTable[MAX_LABELS];
int symbolCount = 0;
char lut1[200][10] ={ "ADC A","ADC B","ADC C","ADC D","ADC E","ADC H","ADC L","ADC M",
                                         "ADD A","ADD B","ADD C","ADD D","ADD E","ADD H","ADD L","ADD M",
                                         "ANA A","ANA B","ANA C","ANA D","ANA E","ANA H","ANA L","ANA M",
                                         "CMP A","CMP B","CMP C","CMP D","CMP E","CMP H","CMP L","CMP M",
                                         "DAD B","DAD D","DAD H","DAD SP",
                                         "DCR A","DCR B","DCR C","DCR D","DCR E","DCR H","DCR L","DCR M",
                                         "DCX B","DCX D","DCX H","DCX SP",
                                         "INR A","INR B","INR C","INR D","INR E","INR H","INR L","INR M",
                                         "INX B","INX D","INX H","INX SP",
                                         "LDAX B","LDAX D",
                                         "MOV A,B","MOV A,C","MOV A,D","MOV A,E","MOV A,H","MOV A,L","MOV A,M",
                                         "MOV B,A","MOV B,C","MOV B,D","MOV B,E","MOV B,H","MOV B,L","MOV B,M",
                                         "MOV C,A","MOV C,B","MOV C,D","MOV C,E","MOV C,H","MOV C,L","MOV C,M",
                                         "MOV D,A","MOV D,B","MOV D,C","MOV D,E","MOV D,H","MOV D,L","MOV D,M",
                                         "MOV E,A","MOV E,B","MOV E,C","MOV E,D","MOV E,H","MOV E,L","MOV E,M",
                                         "MOV H,A","MOV H,B","MOV H,C","MOV H,D","MOV H,E","MOV H,L","MOV H,M",
                                         "MOV L,A","MOV L,B","MOV L,C","MOV L,D","MOV L,E","MOV L,H","MOV L,M",
                                         "MOV M,A","MOV M,B","MOV M,C","MOV M,D","MOV M,E","MOV M,H","MOV M,L",
                                         "ORA A","ORA B","ORA C","ORA D","ORA E","ORA H","ORA L","ORA M",
                                         "POP B","POP D","POP H","POP PSW",
                                         "PUSH B","PUSH D","PUSH H","PUSH PSW",
                                         "RST 0","RST 1","RST 2","RST 3","RST 4","RST 5","RST 6","RST 7",  /*jump statements, so to be done later*/
                                         "SBB A","SBB B","SBB C","SBB D","SBB E","SBB H","SBB L","SBB M",
                                         "SUB A","SUB B","SUB C","SUB D","SUB E","SUB H","SUB L","SUB M",
                                         "STAX B","STAX D",
                                         "XRA A","XRA B","XRA C","XRA D","XRA E","XRA H","XRA L","XRA M",
                                         "HLT","NOP","PCHL","CMA","CMC","DAA","DI","EI",
                                         "RAL","RAR","RC","RET","RIM","RLC","RM","RNC","RNZ","RP","RPE","RPO","RRC",
                                         "RZ","SIM","SPHL","STC","XCHG","XTHL"
                                         };
char lut2[200][7] =  { "8F","88","89","8A","8B","8C","8D","8E",
                                         "87","80","81","82","83","84","85","86",
                                         "A7","A0","A1","A2","A3","A4","A5","A6",
                                         "BF","B8","B9","BA","BB","BC","BD","BE",
                                         "09","19","29","39",
                                         "3D","05","0D","15","1D","25","2D","35",
                                         "0B","1B","2B","3B",
                                         "3C","04","0C","14","1C","24","2C","34",
                                         "03","13","23","33",
                                         "0A","1A",
                                         "78","79","7A","7B","7C","7D","7E",
                                         "47","41","42","43","44","45","46",
                                         "4F","48","4A","4B","4C","4D","4E",
                                         "57","50","51","53","54","55","56",
                                         "5F","58","59","5A","5C","5D","5E",
                                         "67","60","61","62","63","65","66",
                                         "6F","68","69","6A","6B","6C","6E",
                                         "77","70","71","72","73","74","75",
                                         "B7","B0","B1","B2","B3","B4","B5","B6",
                                         "C1","D1","E1","F1",
                                         "C5","D5","E5","F5",
                                         "C7","CF","D7","DF","E7","EF","F7","FF",
                                         "9F","98","99","9A","9B","9C","9D","9E",
                                         "97","90","91","92","93","94","95","96",
                                         "02","12",
                                         "AF","A8","A9","AA","AB","AC","AD","AE",
                                         "76","00","E9","2F","3F","27","F3","FB",
                                         "17","1F","D8","C9","20","07","F8","D0","C0","F0","E8","E0","0F",
                                         "C8","30","F9","37","EB","E3"
                                         };
char lut3[16][7] =   { "ACI","ADI","ANI",                                    //completed and flags updated
                                       "MVI A","MVI B","MVI C","MVI D","MVI E","MVI H","MVI L","MVI M","CPI",
                                       "ORI","SBI","SUI","XRI"
                                     };
char lut4[16][4] =  {  "CE","C6","E6",
                                       "3E","06","0E","16","1E","26","2E","36","FE",
                                       "F6","DE","D6","EE"
                                    };
char lut5[30][7] = {"IN","LDA","LHLD","OUT","SHLD","STA","LXI B","LXI D","LXI H","LXI SP",
                                    "CALL","CC","CM","CNC","CNZ","CPE","CP",
                                    "CPO","CZ","JNC","JMP","JM","JC","JNZ",
                                    "JPE","JP","JPO","JZ"
                                   };
char lut6[30][7] = {"DB","3A","2A","D3","22","32","01","11","21","31",
                                    "CD","DC","FC","D4","C4","EC","F4",
                                    "E4","CC","D2","C3","FA","DA","C2",
                                    "EA","F2","E2","CA"
                                   };


/* Removes leading and trailing white spaces from a string. */
static void trimSpaces(char *line)
{
    char *start = line;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != line) memmove(line, start, strlen(start) + 1);

    int end = (int)strlen(line) - 1;
    while (end >= 0 && isspace((unsigned char)line[end])) line[end--] = '\0';
}

/* Converts text to uppercase in-place. */
static void makeUpperCase(char *line)
{
    for (int i = 0; line[i]; i++) line[i] = (char)toupper((unsigned char)line[i]);
}

/* Removes comments, normalizes spacing, and allows flexible assembly input. */
static void normalizeLine(char *line)
{
    char temp[MAX_LINE_LENGTH];
    int j = 0;

    line[strcspn(line, "\r\n")] = '\0';
    char *comment = strchr(line, ';');
    if (comment) *comment = '\0';
    makeUpperCase(line);
    trimSpaces(line);

    for (int i = 0; line[i] && j < MAX_LINE_LENGTH - 1; i++) {
        char ch = line[i];
        if (ch == '\t') ch = ' ';

        if (ch == ',') {
            while (j > 0 && temp[j - 1] == ' ') j--;
            temp[j++] = ',';
            while (line[i + 1] == ' ' || line[i + 1] == '\t') i++;
        } else if (isspace((unsigned char)ch)) {
            if (j > 0 && temp[j - 1] != ' ' && temp[j - 1] != ',') temp[j++] = ' ';
        } else {
            temp[j++] = ch;
        }
    }
    temp[j] = '\0';
    strcpy(line, temp);
    trimSpaces(line);
}

/* Finds whether a line starts with a complete mnemonic/prefix. */
static int startsWithMnemonic(const char *line, const char *mnemonic)
{
    size_t n = strlen(mnemonic);
    if (strncmp(line, mnemonic, n) != 0) return 0;
    return line[n] == '\0' || line[n] == ' ' || line[n] == ',';
}

/* Parses hex/decimal operands such as 25H, 0x25, 25, or label names. */
static int parseValue(const char *token, unsigned int *value)
{
    char buf[MAX_LINE_LENGTH];
    char *endptr;
    int base = 10;

    if (token == NULL || value == NULL) return 0;
    strncpy(buf, token, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    trimSpaces(buf);
    if (buf[0] == '\0') return 0;

    if (buf[0] == '#') memmove(buf, buf + 1, strlen(buf));
    if (buf[0] == '$') {
        memmove(buf, buf + 1, strlen(buf));
        base = 16;
    }

    int lenBuf = (int)strlen(buf);
    if (lenBuf > 0 && buf[lenBuf - 1] == 'H') {
        buf[lenBuf - 1] = '\0';
        base = 16;
    } else if (lenBuf > 1 && buf[0] == '0' && buf[1] == 'X') {
        base = 16;
    }

    *value = (unsigned int)strtoul(buf, &endptr, base);
    if (endptr && *endptr == '\0') return 1;

    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(buf, symbolTable[i].label) == 0) {
            *value = symbolTable[i].address;
            return 1;
        }
    }
    return 0;
}

/* Writes one byte of machine code into simulator memory. */
static void emitByte(unsigned int byteValue)
{
    if (addr < 0 || addr >= MEMORY_SIZE) {
        printf("ERROR: Address out of range while writing machine code.\n");
        f = 0;
        return;
    }
    sprintf(memory[addr++], "%02X", byteValue & 0xFF);
}

/* Writes a 16-bit value in 8085 little-endian order. */
static void emitWordLittleEndian(unsigned int value)
{
    emitByte(value & 0xFF);
    emitByte((value >> 8) & 0xFF);
}

/* Removes an optional LABEL: prefix from an assembly line. */
static void removeLabel(char *line)
{
    char *colon = strchr(line, ':');
    if (colon) {
        memmove(line, colon + 1, strlen(colon + 1) + 1);
        trimSpaces(line);
    }
}

/* Adds a label and its resolved address into the symbol table. */
static void addSymbol(const char *label, unsigned int address)
{
    if (symbolCount >= MAX_LABELS) {
        printf("ERROR: Too many labels. Increase MAX_LABELS.\n");
        return;
    }

    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].label, label) == 0) {
            symbolTable[i].address = address;
            return;
        }
    }

    strncpy(symbolTable[symbolCount].label, label, MAX_LINE_LENGTH - 1);
    symbolTable[symbolCount].label[MAX_LINE_LENGTH - 1] = '\0';
    symbolTable[symbolCount].address = address;
    symbolCount++;
}

/* Counts comma-separated DB operands. */
static int countDbBytes(const char *operands)
{
    char buf[MAX_LINE_LENGTH];
    int count = 0;
    strncpy(buf, operands, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *token = strtok(buf, ",");
    while (token != NULL) {
        trimSpaces(token);
        if (token[0] != '\0') count++;
        token = strtok(NULL, ",");
    }
    return count;
}

/* Estimates instruction size for the assembler first pass. */
static int instructionSize(char *line)
{
    char temp[MAX_LINE_LENGTH];
    strncpy(temp, line, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    normalizeLine(temp);
    removeLabel(temp);
    if (temp[0] == '\0') return 0;

    if (startsWithMnemonic(temp, "ORG")) return 0;
    if (strstr(temp, " EQU ") != NULL) return 0;
    if (startsWithMnemonic(temp, "DB")) return countDbBytes(temp + 2);

    for (int i = 0; i < 200; i++) {
        if (lut1[i][0] != '\0' && strcmp(temp, lut1[i]) == 0) return 1;
    }
    for (int i = 0; i < 16; i++) {
        if (lut3[i][0] != '\0' && startsWithMnemonic(temp, lut3[i])) return 2;
    }
    if (startsWithMnemonic(temp, "IN") || startsWithMnemonic(temp, "OUT")) return 2;
    for (int i = 0; i < 30; i++) {
        if (lut5[i][0] != '\0' && strcmp(lut5[i], "IN") != 0 && strcmp(lut5[i], "OUT") != 0 && startsWithMnemonic(temp, lut5[i])) return 3;
    }
    return 0;
}

/* Builds label addresses before machine-code generation. */
static void buildSymbolTable(unsigned int startAddress)
{
    unsigned int currentAddress = startAddress;
    symbolCount = 0;

    for (int i = 0; i < len; i++) {
        char line[MAX_LINE_LENGTH];
        strncpy(line, str[i], sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        normalizeLine(line);
        if (line[0] == '\0') continue;

        char *equ = strstr(line, " EQU ");
        if (equ) {
            char label[MAX_LINE_LENGTH];
            unsigned int equValue;
            size_t labelLen = (size_t)(equ - line);
            if (labelLen >= sizeof(label)) labelLen = sizeof(label) - 1;
            strncpy(label, line, labelLen);
            label[labelLen] = '\0';
            trimSpaces(label);
            if (parseValue(equ + 5, &equValue)) addSymbol(label, equValue & 0xFFFF);
            continue;
        }

        char *colon = strchr(line, ':');
        if (colon) {
            char label[MAX_LINE_LENGTH];
            size_t labelLen = (size_t)(colon - line);
            if (labelLen >= sizeof(label)) labelLen = sizeof(label) - 1;
            strncpy(label, line, labelLen);
            label[labelLen] = '\0';
            trimSpaces(label);
            if (label[0] != '\0') addSymbol(label, currentAddress);
            memmove(line, colon + 1, strlen(colon + 1) + 1);
            trimSpaces(line);
        }

        if (startsWithMnemonic(line, "ORG")) {
            unsigned int newAddress;
            if (parseValue(line + 3, &newAddress)) currentAddress = newAddress & 0xFFFF;
            continue;
        }
        currentAddress += (unsigned int)instructionSize(line);
    }
}

/* Checks whether one string is present inside another string. */
char isPresent(char *s1,char *s2)
{
    int M = strlen(s1);
    int N = strlen(s2);
    for (int i = 0; i <= N - M; i++) {
        int j;
        for (j = 0; j < M; j++)
            if (s2[i + j] != s1[j])
                break;
        if (j == M)
            return 1;
    }
    return 0;
}
/* Pushes a 16-bit value on the simulated stack. */
void push(unsigned int x)
{
      char string[10];
      SP--;
      sprintf(string,"%02x",((x>>8)&0xFF));
      strcpy(memory[SP],string);
      SP--;
      sprintf(string,"%02x",(x&0xFF));
      strcpy(memory[SP],string);
}
/* Pops a 16-bit return address from the simulated stack. */
void pop()
{
       unsigned int dec2 = strtol(memory[SP],NULL,16);
       SP++;
       unsigned int tL = dec2&0x00FF;
       dec2 = strtol(memory[SP],NULL,16);
       SP++;
       unsigned int tH = (dec2<<8)&0xFF00;
       PC = ((tH|tL) - 1) & 0xFFFF;                   //copying stack top address to PC
}
/* Swaps two unsigned integer values. */
void swap(unsigned int *a,unsigned int *b)
{
     unsigned int tmp;
     tmp = *a;
     *a = *b;
     *b = tmp;
}
/* Returns 1 when the lower 8 bits have even parity. */
char checkParity(unsigned int n)
{
    int count = 0;
    n = (unsigned char)n;
    while(n){
        count +=n&1;
        n>>=1;
    }
    if(count%2==0) return 1;
    else return 0;
}
/* Updates auxiliary carry/borrow for subtraction. */
void calculateAuxilaryCarry_sub(unsigned int a,unsigned int b)
{
    unsigned char nibble1 = a&0x0F;
    unsigned char nibble2 = b&0x0F;
    unsigned char nibbleSum = nibble1 - nibble2;
    AC = (nibbleSum&0x10)?1:0;
}
/* Updates auxiliary carry for addition. */
void calculateAuxilaryCarry(unsigned int a,unsigned int b)
{
    unsigned char nibble1 = a&0x0F;
    unsigned char nibble2 = b&0x0F;
    unsigned char nibbleSum = nibble1 + nibble2;
    AC = (nibbleSum&0x10)?1:0;
}
/* Updates 8085 flags after arithmetic/logical operations. */
void flagStatusUpdate(unsigned int a)
{
    if((a&0x0100)==0x0100) CF = 1;
    else CF = 0;
    if((a&0x80)==0x80) SF = 1;
    else SF = 0;
    if((a&0xFF)==0) ZF = 1;
    else ZF = 0;
    if(checkParity(a)==1) PF = 1;
    else PF = 0;
    flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
}
/* Updates flags for INR/DCR while preserving carry. */
void DCRflagStatusUpdate(unsigned int a)
{                                                                                       //used for updating flags during INR and DCR operations
    if((a&0x80)==0x80) SF = 1;
    else SF = 0;
    if((a&0xFF)==0) ZF = 1;
    else ZF = 0;
    if(checkParity(a)==1) PF = 1;
    else PF = 0;
    flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
}
/* Updates flags for CMP without changing accumulator. */
void CMPflagStatusUpdate(unsigned int k)
{
    if(A<k) {
            CF = 1;
            SF = 1;
            AC = 0; PF = 0; ZF = 0;
    }
    else if(A>k) {
        CF = 0;
        SF = 0;
        AC = 0; PF = 0; ZF = 0;
    }
    else if(A==k){
        CF = 0;
        SF = 0;
        AC = 0; PF = 1; ZF = 1;
    }
    flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
}
/* Converts one-byte 8085 instructions into machine code. */
void getMachineCode_1(char *ptr,int length)
{
    (void)length;
    char line[MAX_LINE_LENGTH];
    strncpy(line, ptr, sizeof(line) - 1);
    line[sizeof(line) - 1] = '\0';
    normalizeLine(line);
    removeLabel(line);

    if (line[0] == '\0' || startsWithMnemonic(line, "ORG") || strstr(line, " EQU ") != NULL) {
        f = 1;
        return;
    }

    for(int i = 0; i < 200; i++)
    {
        if(lut1[i][0] != '\0' && strcmp(line,lut1[i])==0) {
            printf("%s\n",lut2[i]);
            emitByte((unsigned int)strtoul(lut2[i], NULL, 16));
            f = 1;
            return;
        }
    }
}

/* Converts 8-bit immediate 8085 instructions into machine code. */
void getMachineCode_2(char *ptr,int length)
{
    (void)length;
    char line[MAX_LINE_LENGTH];
    char operand[MAX_LINE_LENGTH];
    unsigned int value;

    strncpy(line, ptr, sizeof(line) - 1);
    line[sizeof(line) - 1] = '\0';
    normalizeLine(line);
    removeLabel(line);

    for(int i = 0; i < 16; i++){
        if(lut3[i][0] != '\0' && startsWithMnemonic(line,lut3[i]))
        {
            const char *op = line + strlen(lut3[i]);
            while (*op == ' ' || *op == ',') op++;
            strncpy(operand, op, sizeof(operand) - 1);
            operand[sizeof(operand) - 1] = '\0';
            trimSpaces(operand);

            if (!parseValue(operand, &value) || value > 0xFF) {
                printf("ERROR: Invalid 8-bit operand '%s' in '%s'\n", operand, line);
                f = 0;
                return;
            }

            printf("%s  %02X\n",lut4[i],value & 0xFF);
            emitByte((unsigned int)strtoul(lut4[i], NULL, 16));
            emitByte(value);
            len1++;
            f = 1;
            return;
        }
    }
}

/* Converts directives, I/O, and 16-bit operand instructions into machine code. */
void getMachineCode_3(char *ptr,int length)
{
    (void)length;
    char line[MAX_LINE_LENGTH];
    char operand[MAX_LINE_LENGTH];
    unsigned int value;

    strncpy(line, ptr, sizeof(line) - 1);
    line[sizeof(line) - 1] = '\0';
    normalizeLine(line);
    removeLabel(line);

    if (line[0] == '\0') {
        f = 1;
        return;
    }

    if (startsWithMnemonic(line, "ORG")) {
        if (parseValue(line + 3, &value)) {
            addr = (int)(value & 0xFFFF);
            f = 1;
        }
        return;
    }

    if (startsWithMnemonic(line, "DB")) {
        char dbcopy[MAX_LINE_LENGTH];
        strncpy(dbcopy, line + 2, sizeof(dbcopy) - 1);
        dbcopy[sizeof(dbcopy) - 1] = '\0';
        char *token = strtok(dbcopy, ",");
        int emitted = 0;
        while (token != NULL) {
            trimSpaces(token);
            if (!parseValue(token, &value) || value > 0xFF) {
                printf("ERROR: Invalid DB byte '%s'\n", token);
                f = 0;
                return;
            }
            printf("%02X\n", value & 0xFF);
            emitByte(value);
            emitted++;
            token = strtok(NULL, ",");
        }
        len1 += emitted;
        f = 1;
        return;
    }

    if (startsWithMnemonic(line, "IN") || startsWithMnemonic(line, "OUT")) {
        const char *opcode = startsWithMnemonic(line, "IN") ? "DB" : "D3";
        const char *op = line + (line[1] == 'N' ? 2 : 3);
        while (*op == ' ' || *op == ',') op++;
        strncpy(operand, op, sizeof(operand) - 1);
        operand[sizeof(operand) - 1] = '\0';
        trimSpaces(operand);
        if (!parseValue(operand, &value) || value > 0xFF) {
            printf("ERROR: Invalid port operand '%s' in '%s'\n", operand, line);
            f = 0;
            return;
        }
        printf("%s  %02X\n", opcode, value & 0xFF);
        emitByte((unsigned int)strtoul(opcode, NULL, 16));
        emitByte(value);
        len1++;
        f = 1;
        return;
    }

    for(int i = 0; i < 30; i++){
        if(lut5[i][0] != '\0' && strcmp(lut5[i], "IN") != 0 && strcmp(lut5[i], "OUT") != 0 && startsWithMnemonic(line,lut5[i]))
        {
            const char *op = line + strlen(lut5[i]);
            while (*op == ' ' || *op == ',') op++;
            strncpy(operand, op, sizeof(operand) - 1);
            operand[sizeof(operand) - 1] = '\0';
            trimSpaces(operand);

            if (!parseValue(operand, &value) || value > 0xFFFF) {
                printf("ERROR: Invalid 16-bit operand/label '%s' in '%s'\n", operand, line);
                f = 0;
                return;
            }

            printf("%s  %02X  %02X\n",lut6[i],value & 0xFF,(value >> 8) & 0xFF);
            emitByte((unsigned int)strtoul(lut6[i], NULL, 16));
            emitWordLittleEndian(value);
            len1 = len1+2;
            f = 1;
            return;
        }
    }
}

/* Prints simulator usage information. */
void init_messages()
{
    printf("XXX-------------------------------------------------8085 SIMULATOR-------------------------------------------------XXX\n\n");
    printf("---------------");
    printf("\nGENERAL RULES:\n");
    printf("---------------\n");
    printf("1. Type everything in UPPER CASE only\n");
    printf("2. Don't leave any unnecessary white spaces\n");
    printf("3. You could able to see only the final status of the registers and flags\n");
    printf("4. Follow the Little endian format while storing in memory\n\n");
}
/* Program entry point: loads memory, assembles input, and runs simulation. */
int main()
{
    char temp[MAX_LINE_LENGTH];
    int tmp = 0;
    (void)tmp;

    for(int mi=0; mi<MEMORY_SIZE; mi++) strcpy(memory[mi],"00");
    for(int pi=0; pi<256; pi++) strcpy(ports[pi],"00");

    init_messages();
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nENTER THE VALUES TO BE STORED IN MEMORY LOCATION (enter NIL if not any and enter END to terminate):\n");
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nFor e.g\n0012H  23\nC301H  1A\n\n");

    while(fgets(temp, sizeof(temp), stdin) != NULL)
    {
        char line[MAX_LINE_LENGTH];
        char addressToken[MAX_LINE_LENGTH], valueToken[MAX_LINE_LENGTH];
        unsigned int n1, n2;

        strncpy(line, temp, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        normalizeLine(line);
        if(line[0] == '\0') continue;
        if(strcmp(line,"END")==0 || strcmp(line,"NIL")==0) break;

        addressToken[0] = valueToken[0] = '\0';
        sscanf(line, "%127s %127s", addressToken, valueToken);
        if(parseValue(addressToken, &n1) && parseValue(valueToken, &n2) && n1 < MEMORY_SIZE && n2 <= 0xFF) {
            sprintf(memory[n1], "%02X", n2 & 0xFF);
        } else {
            printf("WARNING: Ignored invalid memory initialization line: %s\n", line);
        }
    }

    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nENTER THE ASSEMBLY CODE:\n");
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nAccepted examples:\nMOV A,B\nMVI A,96H\nloop: DCR B\nJNZ loop\nDB 01H, 02H\n\n");
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nSTART WITH ORG 2000H OR directly type code. TERMINATE WITH END\n");
    printf("---------------------------------------------------------------------------------------------------------\n");

    len = 0;
    addr = 0;
    program_start_addr = 0;
    int originFound = 0;

    while(fgets(temp, sizeof(temp), stdin) != NULL)
    {
        char line[MAX_LINE_LENGTH];
        unsigned int origin;

        strncpy(line, temp, sizeof(line) - 1);
        line[sizeof(line) - 1] = '\0';
        normalizeLine(line);
        if(line[0] == '\0') continue;
        if(strcmp(line,"END")==0) break;

        if(startsWithMnemonic(line, "ORG")) {
            if(parseValue(line + 3, &origin)) {
                addr = (int)(origin & 0xFFFF);
                if(!originFound) program_start_addr = origin & 0xFFFF;
                originFound = 1;
            } else {
                printf("ERROR: Invalid ORG line: %s\n", line);
                return 1;
            }
            continue;
        }

        if(!originFound) {
            addr = 0;
            program_start_addr = 0;
            originFound = 1;
        }

        if(len >= MAX_LINES) {
            printf("ERROR: Too many assembly lines. Increase MAX_LINES.\n");
            return 1;
        }
        strncpy(str[len], line, MAX_LINE_LENGTH - 1);
        str[len][MAX_LINE_LENGTH - 1] = '\0';
        size[len] = (int)strlen(str[len]);
        len++;
    }

    buildSymbolTable(program_start_addr);
    addr = (int)program_start_addr;
    len1 = 0;

    printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nEQUIVALENT MACHINE CODE\n");
    printf("---------------------------------------------------------------------------------------------------------\n");

    int assembly_error = 0;
    for(int i=0;i<len;i++)
    {
         f = 0;
         getMachineCode_1(str[i],size[i]);
         if(f==0) getMachineCode_2(str[i],size[i]);
         else if(f==1) continue;
         if(f==0)  getMachineCode_3(str[i],size[i]);
         else if(f==1) continue;
         if(f==0){
               printf("ENTER VALID ASSEMBLY CODE near line %d: %s\n", i + 1, str[i]);
               assembly_error = 1;
               break;
        }
    }
    if (assembly_error) return 1;

    program_end_addr = (unsigned int)addr;

    printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nCODE MEMORY IS SHOWN BELOW\n");
    printf("---------------------------------------------------------------------------------------------------------\n");
    for(unsigned int mi=program_start_addr; mi<(unsigned int)addr; mi++) printf("%04X       %s\n",mi,memory[mi]);
    PC = program_start_addr;
    for(;PC<(unsigned int)addr;PC++)                                  //starting from the Program counter (PC)
    {
        //start of ADC operations
        if(strcmp(memory[PC],"8F")==0){
                calculateAuxilaryCarry(A,CF);
                if(AC!=1) calculateAuxilaryCarry(A+A,CF);
                A = A+A+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"88")==0){
                calculateAuxilaryCarry(A,B);
                if(AC!=1) calculateAuxilaryCarry(A+B,CF);
                A = A+B+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"89")==0){
               calculateAuxilaryCarry(A,C);
                if(AC!=1) calculateAuxilaryCarry(A+C,CF);
                A = A+C+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"8A")==0){
                calculateAuxilaryCarry(A,D);
                if(AC!=1) calculateAuxilaryCarry(A+D,CF);
                A = A+D+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"8B")==0){
                calculateAuxilaryCarry(A,E);
                if(AC!=1) calculateAuxilaryCarry(A+E,CF);
                A = A+E+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"8C")==0){
                calculateAuxilaryCarry(A,H);
                if(AC!=1) calculateAuxilaryCarry(A+H,CF);
                A = A+H+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"8D")==0){
                calculateAuxilaryCarry(A,L);
                if(AC!=1) calculateAuxilaryCarry(A+L,CF);
                A = A+L+CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"8E")==0){
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);         //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             calculateAuxilaryCarry(A,dec2);
             if(AC!=1) calculateAuxilaryCarry(A+dec2,CF);
             A = A+dec2+CF;
             flagStatusUpdate(A);
             A = A&0xFF;
        }
        //start of ADD operation
        else if(strcmp(memory[PC],"87")==0){
                calculateAuxilaryCarry(A,A);
                A = A+A;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"80")==0){
                calculateAuxilaryCarry(A,B);
                A = A+B;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"81")==0){
                calculateAuxilaryCarry(A,C);
                A = A+C;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"82")==0){
               calculateAuxilaryCarry(A,D);
                A = A+D;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"83")==0){
                calculateAuxilaryCarry(A,E);
                A = A+E;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"84")==0){
               calculateAuxilaryCarry(A,H);
                A = A+H;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"85")==0){
               calculateAuxilaryCarry(A,L);
                A = A+L;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"86")==0){
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             calculateAuxilaryCarry(A,dec2);
             A = A+dec2;
             flagStatusUpdate(A);
             A = A&0xFF;
        }
        //lets do for ANA instruction
         else if(strcmp(memory[PC],"A7")==0){
             AC  = 1;
             A = A&A;
            flagStatusUpdate(A);
            A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A0")==0){
                AC  = 1;
                A = A&B;
                flagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A1")==0){
                AC  = 1;
                A = A&C;
                flagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A2")==0){
                AC  = 1;
                A = A&D;
                flagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A3")==0){
                AC  = 1;
                A = A&E;
                flagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A4")==0){
               AC  = 1;
                A = A&H;
                flagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A5")==0){
                AC  = 1;
                A = A&L;
                flagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"A6")==0){
             AC = 1;
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             A = A&dec2;
             flagStatusUpdate(A);
             A = A&0xFF;
         }
         //lets do for CMP instruction
         else if(strcmp(memory[PC],"BF")==0) CMPflagStatusUpdate(A);
         else if(strcmp(memory[PC],"B8")==0) CMPflagStatusUpdate(B);
         else if(strcmp(memory[PC],"B9")==0) CMPflagStatusUpdate(C);
         else if(strcmp(memory[PC],"BA")==0) CMPflagStatusUpdate(D);
         else if(strcmp(memory[PC],"BB")==0) CMPflagStatusUpdate(E);
         else if(strcmp(memory[PC],"BC")==0) CMPflagStatusUpdate(H);
         else if(strcmp(memory[PC],"BD")==0) CMPflagStatusUpdate(L);
         else if(strcmp(memory[PC],"BE")==0) {
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             CMPflagStatusUpdate(dec2);
         }
         //lets do for DAD operation
         else if(strcmp(memory[PC],"09")==0){
                unsigned int lp1,lp2,sum;
                calculateAuxilaryCarry(C,L);
                lp1 = (B<<8)|C;
                lp2 = (H<<8)|L;
                sum = lp1+lp2;
                if((sum&0x10000)==0x10000) CF = 1;         //set CF if 17th bit is 1
                else CF = 0;
                L = sum&0x00FF;                                   //storing sum result to HL pair itself
                H = (sum>>8)&0x00FF;
                flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
         }
         else if(strcmp(memory[PC],"19")==0){
                unsigned int lp1,lp2,sum;
                calculateAuxilaryCarry(E,L);
                lp1 = (D<<8)|E;
                lp2 = (H<<8)|L;
                sum = lp1+lp2;
                if((sum&0x10000)==0x10000) CF = 1;         //set CF if 17th bit is 1
                else CF = 0;
                L = sum&0x00FF;                                   //storing sum result to HL pair itself
                H = (sum>>8)&0x00FF;
                flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
         }
         else if(strcmp(memory[PC],"29")==0){
                unsigned int lp1,lp2,sum;
                calculateAuxilaryCarry(L,L);
                lp1 = (H<<8)|L;
                lp2 = (H<<8)|L;
                sum = lp1+lp2;
                if((sum&0x10000)==0x10000) CF = 1;         //set CF if 17th bit is 1
                else CF = 0;
                L = sum&0x00FF;                                   //storing sum result to HL pair itself
                H = (sum>>8)&0x00FF;
                flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
         }
         else if(strcmp(memory[PC],"39")==0){
                unsigned int lp1,lp2,sum;
                calculateAuxilaryCarry(SP&0xFF,L);
                lp1 = SP;
                lp2 = (H<<8)|L;
                sum = lp1 + lp2;
                if((sum&0x10000)==0x10000) CF = 1;         //set CF if 17th bit is 1
                else CF = 0;
                L = sum&0x00FF;                                   //storing sum result to HL pair itself
                H = (sum>>8)&0x00FF;
                flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
         }
         //lets do DCR operation
         else if(strcmp(memory[PC],"3D")==0){
                AC = 1;
                A = A-1;
                DCRflagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"05")==0){
                AC = 1;
                B = B-1;
                DCRflagStatusUpdate(B);
                B = B&0xFF;
         }
         else if(strcmp(memory[PC],"0D")==0){
                AC = 1;
                C= C-1;
                DCRflagStatusUpdate(C);
                C = C&0xFF;
         }
         else if(strcmp(memory[PC],"15")==0){
                AC = 1;
                D = D-1;
                DCRflagStatusUpdate(D);
                D = D&0xFF;
         }
         else if(strcmp(memory[PC],"1D")==0){
                AC = 1;
                E = E-1;
                DCRflagStatusUpdate(E);
                E = E&0xFF;
         }
         else if(strcmp(memory[PC],"25")==0){
                AC = 1;
                H = H-1;
                DCRflagStatusUpdate(H);
                H = H&0xFF;
         }
         else if(strcmp(memory[PC],"2D")==0){
                AC = 1;
                L = L-1;
                DCRflagStatusUpdate(L);
                L = L&0xFF;
         }
         else if(strcmp(memory[PC],"35")==0){
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);         //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             dec2 = dec2 - 1;
             DCRflagStatusUpdate(dec2);
             dec2 = dec2&0xFF;
             sprintf(string,"%02x",dec2);
             strcpy(memory[dec],string);
         }
         //for DCX operation
         else if(strcmp(memory[PC],"0B")==0){
                unsigned int lp;
                lp = (B<<8)|C;
                lp--;
                C= lp&0x00FF;                                   //storing sum result to HL pair itself
                B = (lp>>8)&0x00FF;
         }
         else if(strcmp(memory[PC],"1B")==0){
                unsigned int lp;
                lp = (D<<8)|E;
                lp--;
                E= lp&0x00FF;                                   //storing sum result to HL pair itself
                D = (lp>>8)&0x00FF;
         }
         else if(strcmp(memory[PC],"2B")==0){
                unsigned int lp;
                lp = (H<<8)|L;
                lp--;
                L= lp&0x00FF;                                   //storing sum result to HL pair itself
                H = (lp>>8)&0x00FF;
         }
         else if(strcmp(memory[PC],"3B")==0) SP--;
         //for INR operation
         else if(strcmp(memory[PC],"3C")==0){
                AC = 0;
                A++;
                DCRflagStatusUpdate(A);
                A = A&0xFF;
         }
         else if(strcmp(memory[PC],"04")==0){
                AC = 0;
                B++;
                DCRflagStatusUpdate(B);
                B = B&0xFF;
         }
         else if(strcmp(memory[PC],"0C")==0){
                AC = 0;
                C++;
                DCRflagStatusUpdate(C);
                C = C&0xFF;
         }
         else if(strcmp(memory[PC],"14")==0){
                AC = 0;
                D++;
                DCRflagStatusUpdate(D);
                D = D&0xFF;
         }
         else if(strcmp(memory[PC],"1C")==0){
                AC = 0;
                E++;
                DCRflagStatusUpdate(E);
                E = E&0xFF;
         }
         else if(strcmp(memory[PC],"24")==0){
                AC = 0;
                H++;
                DCRflagStatusUpdate(H);
                H = H&0xFF;
         }
         else if(strcmp(memory[PC],"2C")==0){
                AC = 0;
                L++;
                DCRflagStatusUpdate(L);
                L = L&0xFF;
         }
         else if(strcmp(memory[PC],"34")==0){
             AC = 0;
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             dec2++;
             DCRflagStatusUpdate(dec2);
             dec2 = dec2&0xFF;
             sprintf(string,"%02x",dec2);
             strcpy(memory[dec],string);
         }
         //for INX operation
          else if(strcmp(memory[PC],"03")==0) {
                unsigned int lp;
                lp = (B<<8)|C;
                lp++;
                C= lp&0x00FF;
                B = (lp>>8)&0x00FF;
        }
        else if(strcmp(memory[PC],"13")==0) {                 //in a pair of registers always first one stores higher pair and second one stores lower pair
                unsigned int lp;
                lp = (D<<8)|E;
                lp++;
                E = lp&0x00FF;
                D = (lp>>8)&0x00FF;
        }
        else  if(strcmp(memory[PC],"23")==0) {
                unsigned int lp;
                lp = (H<<8)|L;
                lp++;
                L = lp&0x00FF;
                H = (lp>>8)&0x00FF;
                printf("H and L value is %x and %x\n",H,L);
        }
        else  if(strcmp(memory[PC],"33")==0) SP++;
        //next do for LDAX operation
        else if(strcmp(memory[PC],"0A")==0){
             char string[10];
             sprintf(string,"%02x%02x",B,C);          //combining two characters as string
             int dec = strtol(string,NULL,16);          //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             A = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"1A")==0){
             char string[10];
             sprintf(string,"%02x%02x",D,E);          //combining two characters as string
             int dec = strtol(string,NULL,16);          //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             A = dec2&0xFF;
        }
        //next for MOV operations
        else if(strcmp(memory[PC],"78")==0)  A = B;
        else if(strcmp(memory[PC],"79")==0) A = C;
        else if(strcmp(memory[PC],"7A")==0) A = D;
        else if(strcmp(memory[PC],"7B")==0) A = E;
        else if(strcmp(memory[PC],"7C")==0) A = H;
        else if(strcmp(memory[PC],"7D")==0) A = L;
        else if(strcmp(memory[PC],"7E")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                A = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"47")==0)  B = A;
        else if(strcmp(memory[PC],"41")==0) B = C;
        else if(strcmp(memory[PC],"42")==0) B = D;
        else if(strcmp(memory[PC],"43")==0) B = E;
        else if(strcmp(memory[PC],"44")==0) B = H;
        else if(strcmp(memory[PC],"45")==0) B = L;
        else if(strcmp(memory[PC],"46")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                B = dec2&0xFF;
            }
        else if(strcmp(memory[PC],"4F")==0)  C = A;
        else if(strcmp(memory[PC],"48")==0) C = B;
        else if(strcmp(memory[PC],"4A")==0) C = D;
        else if(strcmp(memory[PC],"4B")==0) C= E;
        else if(strcmp(memory[PC],"4C")==0) C = H;
        else if(strcmp(memory[PC],"4D")==0) C = L;
        else if(strcmp(memory[PC],"4E")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                C = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"57")==0)  D = A;
        else if(strcmp(memory[PC],"50")==0) D = B;
        else if(strcmp(memory[PC],"51")==0) D = C;
        else if(strcmp(memory[PC],"53")==0) D = E;
        else if(strcmp(memory[PC],"54")==0) D = H;
        else if(strcmp(memory[PC],"55")==0) D = L;
        else if(strcmp(memory[PC],"56")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                D = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"5F")==0)  E = A;
        else if(strcmp(memory[PC],"58")==0) E = B;
        else if(strcmp(memory[PC],"59")==0) E = C;
        else if(strcmp(memory[PC],"5A")==0) E = D;
        else if(strcmp(memory[PC],"5C")==0) E = H;
        else if(strcmp(memory[PC],"5D")==0) E = L;
        else if(strcmp(memory[PC],"5E")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                E = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"67")==0)  H = A;
        else if(strcmp(memory[PC],"60")==0) H = B;
        else if(strcmp(memory[PC],"61")==0) H = C;
        else if(strcmp(memory[PC],"62")==0) H = D;
        else if(strcmp(memory[PC],"63")==0) H = E;
        else if(strcmp(memory[PC],"65")==0) H = L;
        else if(strcmp(memory[PC],"66")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                H = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"6F")==0)  L = A;
        else if(strcmp(memory[PC],"68")==0) L = B;
        else if(strcmp(memory[PC],"69")==0) L = C;
        else if(strcmp(memory[PC],"6A")==0) L = D;
        else if(strcmp(memory[PC],"6B")==0) L = E;
        else if(strcmp(memory[PC],"6C")==0) L = H;
        else if(strcmp(memory[PC],"6E")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);          //converting string to hex
                unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
                L = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"77")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",A&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = A
        }
        else if(strcmp(memory[PC],"70")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",B&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = B
        }
        else if(strcmp(memory[PC],"71")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",C&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = C
        }
        else if(strcmp(memory[PC],"72")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",D&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = D
        }
        else if(strcmp(memory[PC],"73")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",E&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = E
        }
        else if(strcmp(memory[PC],"74")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",H&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = H
        }
        else if(strcmp(memory[PC],"75")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%02x",L&0xFF);                       //writing back to memory
                strcpy(memory[dec],string);            //M = L
        }
        //for ORA
        else if(strcmp(memory[PC],"B7")==0){
                 AC = 0;
                 CF = 0;
                 A = A|A;
                 flagStatusUpdate(A);
                 A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B0")==0){
                 AC = 0;
                 CF = 0;
                 A = A|B;
                 flagStatusUpdate(A);
                 A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B1")==0){
                  AC = 0;
                  CF = 0;
                  A = A|C;
                  flagStatusUpdate(A);
                  A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B2")==0){
                  AC = 0;
                  CF = 0;
                  A = A|D;
                  flagStatusUpdate(A);
                  A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B3")==0){
                  AC = 0;
                  CF = 0;
                  A = A|E;
                  flagStatusUpdate(A);
                  A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B4")==0){
                  AC = 0;
                  CF = 0;
                  A = A|H;
                  flagStatusUpdate(A);
                  A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B5")==0){
                  AC = 0;
                  CF = 0;
                  A = A|L;
                  flagStatusUpdate(A);
                  A = A&0xFF;
        }
        else if(strcmp(memory[PC],"B6")==0){
                 AC = 0;
                 CF = 0;
                 char string[10];
                 sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                 int dec = strtol(string,NULL,16);          //converting string to hex
                 unsigned int dec2 = strtol(memory[dec],NULL,16);
                 A = A|dec2;
                 flagStatusUpdate(A);
                 A = A&0xFF;
        }
        //for POP
        else if(strcmp(memory[PC],"C1")==0){
                 unsigned int dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 C = dec2&0xFF;
                 dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 B = dec2&0xFF;
            }
        else if(strcmp(memory[PC],"D1")==0){
                 unsigned int dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 E = dec2&0xFF;
                 dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 D = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"E1")==0){
                 unsigned int dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 L = dec2&0xFF;
                 dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 H = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"F1")==0){
                unsigned int dec2 = strtol(memory[SP],NULL,16);
                SP++;
                flagRegister = dec2&0xFF;
                dec2 = strtol(memory[SP],NULL,16);
                SP++;
                A = dec2&0xFF;
        }
            //for PUSH
        else if(strcmp(memory[PC],"C5")==0){
                   char string[10];
                   SP--;
                   sprintf(string,"%02x",B);
                   strcpy(memory[SP],string);
                   SP--;
                   sprintf(string,"%02x",C);
                   strcpy(memory[SP],string);
        }
        else if(strcmp(memory[PC],"D5")==0){
                   char string[10];
                   SP--;
                   sprintf(string,"%02x",D);
                   strcpy(memory[SP],string);
                   SP--;
                   sprintf(string,"%02x",E);
                   strcpy(memory[SP],string);
        }
        else if(strcmp(memory[PC],"E5")==0){
                   char string[10];
                   SP--;
                   sprintf(string,"%02x",H);
                   strcpy(memory[SP],string);
                   SP--;
                   sprintf(string,"%02x",L);
                   strcpy(memory[SP],string);
        }
        else if(strcmp(memory[PC],"F5")==0){
                   char string[10];
                   SP--;
                   sprintf(string,"%02x",A);
                   strcpy(memory[SP],string);
                   SP--;
                   sprintf(string,"%02x",flagRegister);
                   strcpy(memory[SP],string);
        }
            //for SBB
        else if(strcmp(memory[PC],"9F")==0) {
                calculateAuxilaryCarry_sub(A,A);
                if(AC!=1) calculateAuxilaryCarry_sub(A-A,CF);
                A = A-A-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"98")==0) {
                calculateAuxilaryCarry_sub(A,B);
                if(AC!=1) calculateAuxilaryCarry_sub(A-B,CF);
                A = A-B-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"99")==0) {
                calculateAuxilaryCarry_sub(A,C);
                if(AC!=1) calculateAuxilaryCarry_sub(A-C,CF);
                A = A-C-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"9A")==0) {
                calculateAuxilaryCarry_sub(A,D);
                if(AC!=1) calculateAuxilaryCarry_sub(A-D,CF);
                A = A-D-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"9B")==0) {
                calculateAuxilaryCarry_sub(A,E);
                if(AC!=1) calculateAuxilaryCarry_sub(A-E,CF);
                A = A-E-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"9C")==0) {
                calculateAuxilaryCarry_sub(A,H);
                if(AC!=1) calculateAuxilaryCarry_sub(A-H,CF);
                A = A-H-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"9D")==0){
                calculateAuxilaryCarry_sub(A,L);
                if(AC!=1) calculateAuxilaryCarry_sub(A-L,CF);
                A = A-L-CF;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"9E")==0){
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             calculateAuxilaryCarry_sub(A,dec2);
             if(AC!=1) calculateAuxilaryCarry_sub(A-dec2,CF);
             A = A-dec2-CF;
             flagStatusUpdate(A);
             A = A&0xFF;
        }
        //for SUB
        else if(strcmp(memory[PC],"97")==0) {
                calculateAuxilaryCarry_sub(A,A);
                A = A-A;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"90")==0) {
                calculateAuxilaryCarry_sub(A,B);
                A = A-B;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"91")==0) {
                calculateAuxilaryCarry_sub(A,C);
                A = A-C;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"92")==0) {
                calculateAuxilaryCarry_sub(A,D);
                A = A-D;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"93")==0) {
                calculateAuxilaryCarry_sub(A,E);
                A = A-E;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"94")==0) {
                calculateAuxilaryCarry_sub(A,H);
                A = A-H;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"95")==0){
                calculateAuxilaryCarry_sub(A,L);
                A = A-L;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"96")==0){
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string, H and L together stores address
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             calculateAuxilaryCarry_sub(A,dec2);
             A = A-dec2;
             flagStatusUpdate(A);
             A = A&0xFF;
        }
        //for STAX
        else if(strcmp(memory[PC],"02")==0){
                 char string[10];
                 sprintf(string,"%02x%02x",B,C);          //combining two characters as string, H and L together stores address
                 int dec = strtol(string,NULL,16);
                 sprintf(string,"%02x",A);
                 strcpy(memory[dec],string);
        }
        else if(strcmp(memory[PC],"12")==0){
                 char string[10];
                 sprintf(string,"%02x%02x",D,E);          //combining two characters as string, H and L together stores address
                 int dec = strtol(string,NULL,16);
                 sprintf(string,"%02x",A);
                 strcpy(memory[dec],string);
        }
        //for XRA
        else if(strcmp(memory[PC],"AF")==0) {
                AC = 0; CF = 0;
                A = A^A;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else if(strcmp(memory[PC],"A8")==0) {
                A = A^B;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"A9")==0) {
                AC = 0; CF = 0;
                A = A^C;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AA")==0) {
                AC = 0; CF = 0;
                A = A^D;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AB")==0) {
                AC = 0; CF = 0;
                A = A^E;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AC")==0) {
                AC = 0; CF = 0;
                A = A^H;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AD")==0){
                A = A^L;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AE")==0){
             AC = 0; CF = 0;
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string, H and L together stores address
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             A = A^dec2;
             flagStatusUpdate(A);
             A = A&0xFF;
        }
        //for CMA
        else if(strcmp(memory[PC],"2F")==0) A = (~A)&0xFF;
        //for CMC
        else if(strcmp(memory[PC],"3F")==0){
            if(CF == 1) CF = 0;
            else if(CF == 0) CF = 1;
         }
         //for DAA
         else if(strcmp(memory[PC],"27")==0){
              if(((A&0xF0)>0x90)&&((A&0x0F)>0x09)) {
                    calculateAuxilaryCarry(A,0x66);
                    A = A + 0x66;
              }
              else if((A&0x0F)>0x09) {
                    calculateAuxilaryCarry(A,0x06);
                    A=A+0x06;
              }
              else if((A&0xF0)>0x90) {
                    calculateAuxilaryCarry(A,0x60);
                    A = A+0x60;
              }
              flagStatusUpdate(A);
              A = A&0xFF;
              flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);
        }
        //for RAL
       else if(strcmp(memory[PC],"17")==0){           //rotate to left
            unsigned int oldCarry = CF;
            CF = (A >> 7) & 0x01;
            A = ((A << 1) | oldCarry) & 0xFF;               //since registers only the 8 bits, so we are typecasting to store first 8 bits alone
            flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
       }
       //for RAR
       else if(strcmp(memory[PC],"1F")==0){            //rotate to right
            unsigned int oldCarry = CF;
            CF = A & 0x01;
            A = ((A >> 1) | (oldCarry << 7)) & 0xFF;
            flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
       }
       //for RLC
       else if(strcmp(memory[PC],"07")==0){
         A = A<<1;
         if((A&0x0100)==0x0100) {
                CF = 1;
                A = (A&0xFF)|0x01;
         }
         else {
                CF = 0;
                A = A&0xFF;
         }
         flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
    }
    //for RRC
    else if(strcmp(memory[PC],"0F")==0){
         if((A&0x01)==0x01) {
                CF = 1;
                A = ((A>>1)&0xFF)|0x80;
         }
         else{
                CF = 0;
                A = (A>>1)&0x7F;
         }
         flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
    }
    //for SPHL
    else if(strcmp(memory[PC],"F9")==0){
         SP = ((H<<8)&0xFF00)|(L&0x00FF);
    }
    //for STC
    else if(strcmp(memory[PC],"37")==0){
        CF = 1;
        flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
    }
    //for XCHG
    else if(strcmp(memory[PC],"EB")==0){
        swap(&H,&D);
        swap(&L,&E);
    }
    //for XTHL
    else if(strcmp(memory[PC],"E3")==0){
        char string[10];
        unsigned int tH,tL;
        unsigned int dec2 = strtol(memory[SP],NULL,16);
        SP++;
        tL = dec2&0x00FF;
        dec2 = strtol(memory[SP],NULL,16);
        SP++;
        tH = (dec2)&0x00FF;
        swap(&tH,&H);
        swap(&tL,&L);
        SP--;
        sprintf(string,"%02x",tH);
        strcpy(memory[SP],string);
        SP--;
        sprintf(string,"%02x",tL);
        strcpy(memory[SP],string);
       }
       //for RST instructions
       else if(strcmp(memory[PC],"C7")==0){ push(PC+1); PC = 0x00-1; }
       else if(strcmp(memory[PC],"CF")==0){ push(PC+1); PC = 0x08-1; }
       else if(strcmp(memory[PC],"D7")==0){ push(PC+1); PC = 0x10-1; }
       else if(strcmp(memory[PC],"DF")==0){ push(PC+1); PC = 0x18-1; }
       else if(strcmp(memory[PC],"E7")==0){ push(PC+1); PC = 0x20-1; }
       else if(strcmp(memory[PC],"EF")==0){ push(PC+1); PC = 0x28-1; }
       else if(strcmp(memory[PC],"F7")==0){ push(PC+1); PC = 0x30-1; }
       else if(strcmp(memory[PC],"FF")==0){ push(PC+1); PC = 0x38-1; }
       //for HLT
       else if(strcmp(memory[PC],"76")==0) break;            //ending the program
       //RET
       else if(strcmp(memory[PC],"C9")==0) pop();            //copying stack top to PC
       //PCHL
       else if(strcmp(memory[PC],"E9")==0) PC = (((H<<8)&0xFF00)|(L&0x00FF))-1;
       //RC
       else if(strcmp(memory[PC],"D8")==0) {
             if(CF==1) pop();
       }
       //RM
       else if(strcmp(memory[PC],"F8")==0) {
             if(SF==1) pop();
       }
       //RNC
       else if(strcmp(memory[PC],"D0")==0) {
             if(CF==0) pop();
       }
       //RNZ
       else if(strcmp(memory[PC],"C0")==0) {
             if(ZF==0) pop();
       }
       //RP
       else if(strcmp(memory[PC],"F0")==0) {
             if(SF==0) pop();
       }
       //RPE
       else if(strcmp(memory[PC],"E8")==0) {
             if(PF==1) pop();
       }
       //RPO
       else if(strcmp(memory[PC],"E0")==0) {
             if(PF==0) pop();
       }
       //RZ
       else if(strcmp(memory[PC],"C8")==0) {
             if(ZF==1) pop();
       }
       //end of without operand instructions
       //start for ACI
       else if(strcmp(memory[PC],"CE")==0){
           unsigned int temp;
           sscanf(memory[PC+1],"%x",&temp);                     //converting string to hex number
           PC++;
           calculateAuxilaryCarry(A,(temp+CF));
           A = A+temp+CF;
           flagStatusUpdate(A);
           A = A&0xFF;
       }
       //CPI
       else if(strcmp(memory[PC],"FE")==0){
           unsigned int temp;
           sscanf(memory[PC+1],"%x",&temp);                     //converting string to hex number
           PC++;
           calculateAuxilaryCarry(A,temp);
           unsigned int t = A - temp;
           flagStatusUpdate(t);
       }
       //ADI
       else if(strcmp(memory[PC],"C6")==0){
            unsigned int temp;
            sscanf(memory[PC+1],"%x",&temp);
            PC++;
            calculateAuxilaryCarry(A,temp);
            A = A+temp;
            flagStatusUpdate(A);
            A = A&0xFF;
       }
    //ANI
       else if(strcmp(memory[PC],"E6")==0){
            unsigned int temp;
            sscanf(memory[PC+1],"%x",&temp);
            PC++;
            calculateAuxilaryCarry_sub(A,temp);
            A = A&temp;
            flagStatusUpdate(A);
            A = A&0xFF;
        }
        //MVI
        else if(strcmp(memory[PC],"3E")==0){
              unsigned int temp;
              sscanf(memory[PC+1],"%x",&temp);
              PC++;
              temp = temp&0xFF;
              A = temp;
        }
        else if(strcmp(memory[PC],"06")==0){
            unsigned int temp;
            sscanf(memory[PC+1],"%x",&temp);
            PC++;
            temp = temp&0xFF;
            B = temp;
       }
       else if(strcmp(memory[PC],"0E")==0){
            unsigned int temp;
            sscanf(memory[PC+1],"%x",&temp);
            PC++;
            temp = temp&0xFF;
            C = temp;
     }
      else if(strcmp(memory[PC],"16")==0){
            unsigned int temp;
            sscanf(memory[PC+1],"%x",&temp);
            PC++;
            temp = temp&0xFF;
            D = temp;
      }
      else if(strcmp(memory[PC],"1E")==0){
           unsigned int temp;
           sscanf(memory[PC+1],"%x",&temp);
            PC++;
           temp = temp&0xFF;
           E = temp;
       }
       else if(strcmp(memory[PC],"26")==0){
            unsigned int temp;
            sscanf(memory[PC+1],"%x",&temp);
            PC++;
            temp = temp&0xFF;
            H = temp;
       }
       else if(strcmp(memory[PC],"2E")==0){
             unsigned int temp;
             sscanf(memory[PC+1],"%x",&temp);
             PC++;
             temp = temp&0xFF;
             L = temp;
        }
        else if(strcmp(memory[PC],"36")==0){
             char string[10];
             sprintf(string,"%02x%02x",H,L);
             int dec = strtol(string,NULL,16);
             strcpy(memory[dec],memory[PC+1]);
             PC++;
        }
        //ORI
       else if(strcmp(memory[PC],"F6")==0){
               unsigned int temp;
               sscanf(memory[PC+1],"%x",&temp);
               PC++;
               temp &=0xFF;
               A = A|temp;
               flagStatusUpdate(A);
               A &=0xFF;
               AC = 0;
        }
        //SBI
        else if(strcmp(memory[PC],"DE")==0){
              unsigned int temp;
              sscanf(memory[PC+1],"%x",&temp);
              PC++;
              temp &=0xFF;
              calculateAuxilaryCarry_sub(A-temp,CF);
              A = A-temp-CF;
              flagStatusUpdate(A);
              A = A&0xFF;
         }
         //SUI
        else if(strcmp(memory[PC],"D6")==0){
              unsigned int temp;
              sscanf(memory[PC+1],"%x",&temp);
              PC++;
              temp &= 0xFF;
              calculateAuxilaryCarry_sub(A,temp);
              A = A - temp;
              flagStatusUpdate(A);
              A = A&0xFF;
         }
         //XRI
       else if(strcmp(memory[PC],"EE")==0){
             unsigned int temp;
             sscanf(memory[PC+1],"%x",&temp);
             PC++;
             temp &= 0xFF;
             A = A^temp;
             flagStatusUpdate(A);
             A = A&0xFF;
             AC = 0;
    }
    //IN
    else if(strcmp(memory[PC],"DB")==0){
             unsigned int temp;
             sscanf(memory[PC+1],"%x",&temp);
             PC++;
             A = (unsigned int)strtoul(ports[temp & 0xFF], NULL, 16) & 0xFF;
    }
    //OUT
    else if(strcmp(memory[PC],"D3")==0){
             unsigned int temp;
             sscanf(memory[PC+1],"%x",&temp);
             PC++;
             sprintf(ports[temp & 0xFF], "%02x", A & 0xFF);
    }
    //end of 1 byte operand instructions
    //LDA
    else if(strcmp(memory[PC],"3A")==0){
           unsigned int temp;
           char strH[3], strL[3];
           strcpy(strL,memory[PC+1]);
           strcpy(strH,memory[PC+2]);
           PC +=2;
           strcat(strH,strL);
           sscanf(strH,"%x",&temp);                                                             //converting string to hex
           unsigned int dec = strtol(memory[temp],NULL,16);              //getting data from memory
           A = dec&0xFF;
    }
    //LHLD
    else if(strcmp(memory[PC],"2A")==0){
           unsigned int temp;
           char strH[3], strL[3];
           strcpy(strL,memory[PC+1]);
           strcpy(strH,memory[PC+2]);
           PC +=2;
           strcat(strH,strL);
           sscanf(strH,"%x",&temp);
           unsigned int dec1 = strtol(memory[temp],NULL,16);
           temp++;
           unsigned int dec2 = strtol(memory[temp],NULL,16);
           L = dec1&0xFF;
           H = dec2&0xFF;
    }
    //SHLD
    else if(strcmp(memory[PC],"22")==0){
           unsigned int temp;
           char strH[3], strL[3];
           strcpy(strL,memory[PC+1]);
           strcpy(strH,memory[PC+2]);
           PC +=2;
           strcat(strH,strL);
           sscanf(strH,"%x",&temp);
           sprintf(memory[temp],"%02X",L&0xFF);
           temp++;
           sprintf(memory[temp],"%02X",H&0xFF);
    }
    //STA
    else if(strcmp(memory[PC],"32")==0){               //storing accumulator contents to memory stored in operand_2byte
            unsigned int temp;
            char strH[3], strL[3];
           strcpy(strL,memory[PC+1]);
           strcpy(strH,memory[PC+2]);
           PC +=2;
           strcat(strH,strL);
           sscanf(strH,"%x",&temp);
           sprintf(memory[temp],"%02X",A&0xFF);
    }
    //LXI
    else if(strcmp(memory[PC],"01")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              C = temp&0xFF;
              temp = temp>>8;
              B = temp&0xFF;
        }
    else if(strcmp(memory[PC],"11")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              E = temp&0xFF;
              temp = temp>>8;
              D = temp&0xFF;
        }
    else if(strcmp(memory[PC],"21")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              L = temp&0xFF;
              temp = temp>>8;
              H = temp&0xFF;
        }
    else if(strcmp(memory[PC],"31")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              SP = temp;
              SP = SP&0xFFFF;
     }
     //start of JUMP and CALL instructions
     //JC
     else if(strcmp(memory[PC],"DA")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(CF==1) PC = temp-1;
     }
     //JM
     else if(strcmp(memory[PC],"FA")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(SF==1) PC = temp-1;
     }
     //JMP
     else if(strcmp(memory[PC],"C3")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              PC = temp-1;
     }
     //JNC
     else if(strcmp(memory[PC],"D2")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(CF==0) PC = temp-1;
     }
     //JNZ
     else if(strcmp(memory[PC],"C2")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(ZF==0) PC = temp-1;
     }
     //JP
     else if(strcmp(memory[PC],"F2")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(SF==0) PC = temp-1;
     }
      //JPE
     else if(strcmp(memory[PC],"EA")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(PF==1) PC = temp-1;
     }
     //JPO
     else if(strcmp(memory[PC],"E2")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(PF==0) PC = temp-1;
     }
     //JZ
     else if(strcmp(memory[PC],"CA")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);                                          //now strH stores the entire 16 bit value (data or address)
              sscanf(strH,"%x",&temp);
              if(ZF==1) PC = temp-1;
     }
     //CALL
     else if(strcmp(memory[PC],"CD")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              PC = temp-1;
     }
     //CC
     else if(strcmp(memory[PC],"DC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(CF==1){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CM
     else if(strcmp(memory[PC],"FC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(SF==1){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CNC
     else if(strcmp(memory[PC],"D4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(CF==0){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CNZ
     else if(strcmp(memory[PC],"C4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(ZF==0){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CP
     else if(strcmp(memory[PC],"F4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(SF==0){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CPE
     else if(strcmp(memory[PC],"EC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(PF==1){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CPO
     else if(strcmp(memory[PC],"E4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(PF==0){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
     //CZ
     else if(strcmp(memory[PC],"CC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(ZF==1){
                  push(PC+1);               //pushing next address to stack
                  PC = temp-1;
              }
     }
 }
    printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nFINAL STATUS OF REGISTERS\n");
    printf("---------------------------------------------------------------------------------------------------------\n");
    printf("A  ---> %02x\n",A); printf("B  ---> %02x\n",B); printf("C  ---> %02x\n",C); printf("D  ---> %02x\n",D);
    printf("E  ---> %02x\n",E); printf("H  ---> %02x\n",H); printf("L  ---> %02x\n",L); printf("SP  ---> %04x\n",SP&0xFFFF);
    printf("PC  ---> %04x\n",PC+1);
     printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nFINAL STATUS OF FLAGS\n");
     printf("---------------------------------------------------------------------------------------------------------\n");
    printf("ZF ---> %d\n",ZF); printf("SF ---> %d\n",SF); printf("PF ---> %d\n",PF); printf("CF ---> %d\n",CF);printf("AC ---> %d\n",AC);
    //printf("memory storage is %s\n",memory[0x0100]);
    printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nENTER ANY REQUIRED MEMORY LOCATIONS TO VIEW AND TYPE -1 TO TERMINATE (e.g. 1004H)\n");
     printf("---------------------------------------------------------------------------------------------------------\n");
    while(1)
    {
         char line[MAX_LINE_LENGTH];
         unsigned int h;
         if (fgets(line, sizeof(line), stdin) == NULL) break;
         normalizeLine(line);
         if(strcmp(line,"-1")==0) break;
         if(parseValue(line, &h) && h < MEMORY_SIZE) printf("%04X      %s\n",h,memory[h]);
         else printf("Invalid memory address. Try like 1004H or type -1.\n");
    }

}

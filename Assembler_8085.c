//8085 assembler has 16 bit of address bus so memory can be indicated by 2 bytes
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define MAX_LINES 100
#define MAX_LINE_LEN 120
#define MAX_LABELS 100

char str[MAX_LINES][MAX_LINE_LEN],f=0;
int size[MAX_LINES];
int addr_array[20],m=-1;               //stack storing the address - kept from original code structure
int len,address;

typedef struct {
    char mnemonic[20];
    char opcode[4];
} OPCODE_ENTRY;

typedef struct {
    char name[30];
    int address;
} LABEL_ENTRY;

LABEL_ENTRY labels[MAX_LABELS];
int label_count = 0;

/* 1-byte 8085 instructions */
OPCODE_ENTRY lut1[] = {
    {"NOP","00"},{"STAX B","02"},{"INX B","03"},{"INR B","04"},{"DCR B","05"},{"RLC","07"},
    {"DAD B","09"},{"LDAX B","0A"},{"DCX B","0B"},{"INR C","0C"},{"DCR C","0D"},{"RRC","0F"},
    {"STAX D","12"},{"INX D","13"},{"INR D","14"},{"DCR D","15"},{"RAL","17"},
    {"DAD D","19"},{"LDAX D","1A"},{"DCX D","1B"},{"INR E","1C"},{"DCR E","1D"},{"RAR","1F"},
    {"RIM","20"},{"INX H","23"},{"INR H","24"},{"DCR H","25"},{"DAA","27"},
    {"DAD H","29"},{"DCX H","2B"},{"INR L","2C"},{"DCR L","2D"},{"CMA","2F"},
    {"SIM","30"},{"INX SP","33"},{"INR M","34"},{"DCR M","35"},{"STC","37"},
    {"DAD SP","39"},{"DCX SP","3B"},{"INR A","3C"},{"DCR A","3D"},{"CMC","3F"},

    {"MOV B,B","40"},{"MOV B,C","41"},{"MOV B,D","42"},{"MOV B,E","43"},{"MOV B,H","44"},{"MOV B,L","45"},{"MOV B,M","46"},{"MOV B,A","47"},
    {"MOV C,B","48"},{"MOV C,C","49"},{"MOV C,D","4A"},{"MOV C,E","4B"},{"MOV C,H","4C"},{"MOV C,L","4D"},{"MOV C,M","4E"},{"MOV C,A","4F"},
    {"MOV D,B","50"},{"MOV D,C","51"},{"MOV D,D","52"},{"MOV D,E","53"},{"MOV D,H","54"},{"MOV D,L","55"},{"MOV D,M","56"},{"MOV D,A","57"},
    {"MOV E,B","58"},{"MOV E,C","59"},{"MOV E,D","5A"},{"MOV E,E","5B"},{"MOV E,H","5C"},{"MOV E,L","5D"},{"MOV E,M","5E"},{"MOV E,A","5F"},
    {"MOV H,B","60"},{"MOV H,C","61"},{"MOV H,D","62"},{"MOV H,E","63"},{"MOV H,H","64"},{"MOV H,L","65"},{"MOV H,M","66"},{"MOV H,A","67"},
    {"MOV L,B","68"},{"MOV L,C","69"},{"MOV L,D","6A"},{"MOV L,E","6B"},{"MOV L,H","6C"},{"MOV L,L","6D"},{"MOV L,M","6E"},{"MOV L,A","6F"},
    {"MOV M,B","70"},{"MOV M,C","71"},{"MOV M,D","72"},{"MOV M,E","73"},{"MOV M,H","74"},{"MOV M,L","75"},{"HLT","76"},{"MOV M,A","77"},
    {"MOV A,B","78"},{"MOV A,C","79"},{"MOV A,D","7A"},{"MOV A,E","7B"},{"MOV A,H","7C"},{"MOV A,L","7D"},{"MOV A,M","7E"},{"MOV A,A","7F"},

    {"ADD B","80"},{"ADD C","81"},{"ADD D","82"},{"ADD E","83"},{"ADD H","84"},{"ADD L","85"},{"ADD M","86"},{"ADD A","87"},
    {"ADC B","88"},{"ADC C","89"},{"ADC D","8A"},{"ADC E","8B"},{"ADC H","8C"},{"ADC L","8D"},{"ADC M","8E"},{"ADC A","8F"},
    {"SUB B","90"},{"SUB C","91"},{"SUB D","92"},{"SUB E","93"},{"SUB H","94"},{"SUB L","95"},{"SUB M","96"},{"SUB A","97"},
    {"SBB B","98"},{"SBB C","99"},{"SBB D","9A"},{"SBB E","9B"},{"SBB H","9C"},{"SBB L","9D"},{"SBB M","9E"},{"SBB A","9F"},
    {"ANA B","A0"},{"ANA C","A1"},{"ANA D","A2"},{"ANA E","A3"},{"ANA H","A4"},{"ANA L","A5"},{"ANA M","A6"},{"ANA A","A7"},
    {"XRA B","A8"},{"XRA C","A9"},{"XRA D","AA"},{"XRA E","AB"},{"XRA H","AC"},{"XRA L","AD"},{"XRA M","AE"},{"XRA A","AF"},
    {"ORA B","B0"},{"ORA C","B1"},{"ORA D","B2"},{"ORA E","B3"},{"ORA H","B4"},{"ORA L","B5"},{"ORA M","B6"},{"ORA A","B7"},
    {"CMP B","B8"},{"CMP C","B9"},{"CMP D","BA"},{"CMP E","BB"},{"CMP H","BC"},{"CMP L","BD"},{"CMP M","BE"},{"CMP A","BF"},

    {"RNZ","C0"},{"POP B","C1"},{"PUSH B","C5"},{"RST 0","C7"},{"RZ","C8"},{"RET","C9"},{"RST 1","CF"},
    {"RNC","D0"},{"POP D","D1"},{"PUSH D","D5"},{"RST 2","D7"},{"RC","D8"},{"RST 3","DF"},
    {"RPO","E0"},{"POP H","E1"},{"XTHL","E3"},{"PUSH H","E5"},{"RST 4","E7"},{"RPE","E8"},{"PCHL","E9"},{"XCHG","EB"},{"RST 5","EF"},
    {"RP","F0"},{"POP PSW","F1"},{"DI","F3"},{"PUSH PSW","F5"},{"RST 6","F7"},{"RM","F8"},{"SPHL","F9"},{"EI","FB"},{"RST 7","FF"}
};

/* 2-byte instructions: opcode + 8-bit data/port */
OPCODE_ENTRY lut3[] = {
    {"ACI","CE"},{"ADI","C6"},{"ANI","E6"},{"CPI","FE"},{"IN","DB"},
    {"MVI A","3E"},{"MVI B","06"},{"MVI C","0E"},{"MVI D","16"},{"MVI E","1E"},{"MVI H","26"},{"MVI L","2E"},{"MVI M","36"},
    {"ORI","F6"},{"OUT","D3"},{"SBI","DE"},{"SUI","D6"},{"XRI","EE"}
};

/* 3-byte instructions: opcode + lower address byte + higher address byte */
OPCODE_ENTRY lut5[] = {
    {"LDA","3A"},{"LHLD","2A"},{"SHLD","22"},{"STA","32"},
    {"LXI B","01"},{"LXI D","11"},{"LXI H","21"},{"LXI SP","31"},
    {"CALL","CD"},{"CC","DC"},{"CM","FC"},{"CNC","D4"},{"CNZ","C4"},{"CP","F4"},{"CPE","EC"},{"CPO","E4"},{"CZ","CC"},
    {"JC","DA"},{"JM","FA"},{"JMP","C3"},{"JNC","D2"},{"JNZ","C2"},{"JP","F2"},{"JPE","EA"},{"JPO","E2"},{"JZ","CA"}
};

int lut1_count = sizeof(lut1) / sizeof(lut1[0]);
int lut3_count = sizeof(lut3) / sizeof(lut3[0]);
int lut5_count = sizeof(lut5) / sizeof(lut5[0]);

/*
 * Removes leading and trailing white spaces from the given string.
 * This helps normalize every input line before parsing.
 */
void trim(char *s)
{
    int start = 0;
    int end = (int)strlen(s) - 1;

    while(s[start] && isspace((unsigned char)s[start]))
        start++;

    while(end >= start && isspace((unsigned char)s[end]))
        s[end--] = '\0';

    if(start > 0)
        memmove(s, s + start, strlen(s + start) + 1);
}

/*
 * Converts the input string to uppercase.
 * Characters inside single or double quotes are not modified, so DB strings remain unchanged.
 */
void toUpperString(char *s)
{
    int i;
    char quote = 0;

    for(i=0;s[i];i++) {
        if((s[i] == '\'' || s[i] == '"') && quote == 0)
            quote = s[i];
        else if(s[i] == quote)
            quote = 0;

        if(quote == 0)
            s[i] = (char)toupper((unsigned char)s[i]);
    }
}

/*
 * Removes comments from an assembly line.
 * Supports ';' comments and '//' comments while ignoring comment symbols inside quotes.
 */
void removeComment(char *s)
{
    int i;
    char quote = 0;

    for(i=0;s[i];i++) {
        if((s[i] == '\'' || s[i] == '"') && quote == 0)
            quote = s[i];
        else if(s[i] == quote)
            quote = 0;

        if(quote == 0 && s[i] == ';') {
            s[i] = '\0';
            return;
        }

        if(quote == 0 && s[i] == '/' && s[i+1] == '/') {
            s[i] = '\0';
            return;
        }
    }
}

/*
 * Cleans one assembly line before processing.
 * It removes comments, trims spaces, and converts instructions/directives to uppercase.
 */
void cleanLine(char *s)
{
    removeComment(s);
    trim(s);
    toUpperString(s);
}

/*
 * Checks whether the given string is a valid label name.
 * A valid label starts with an alphabet or underscore and contains only alphanumeric characters or underscores.
 */
int isValidLabelName(char *s)
{
    int i;
    if(s[0] == '\0')
        return 0;
    if(!(isalpha((unsigned char)s[0]) || s[0] == '_'))
        return 0;
    for(i=1;s[i];i++) {
        if(!(isalnum((unsigned char)s[i]) || s[i] == '_'))
            return 0;
    }
    return 1;
}

/*
 * Searches the label table for a label name.
 * Returns the label address if found, otherwise returns -1.
 */
int findLabel(char *name)
{
    int i;
    for(i=0;i<label_count;i++) {
        if(strcmp(labels[i].name,name)==0)
            return labels[i].address;
    }
    return -1;
}

/*
 * Adds a new label and its address into the label table.
 * If the label already exists, this function silently skips adding it again.
 */
void addLabel(char *name,int addr)
{
    if(findLabel(name) != -1)
        return;

    if(label_count < MAX_LABELS) {
        strcpy(labels[label_count].name,name);
        labels[label_count].address = addr & 0xFFFF;
        label_count++;
    }
}

/* Removes LABEL: from the beginning of a line, if present */
/*
 * Detects and removes a label from the beginning of a line.
 * If a valid LABEL: is found, it stores the label with the current address and keeps only the instruction part.
 */
void removeLabelFromLine(char *line,int currentAddress)
{
    char *colon;
    char label[30];

    colon = strchr(line,':');
    if(colon == NULL)
        return;

    strncpy(label,line,colon-line);
    label[colon-line] = '\0';
    trim(label);

    if(isValidLabelName(label)) {
        addLabel(label,currentAddress);
        memmove(line,colon+1,strlen(colon+1)+1);
        trim(line);
    }
}

/*
 * Checks whether a line starts with a complete assembler word/directive.
 * This avoids false matches where one mnemonic is only a prefix of another word.
 */
int startsWithWord(char *line,char *word)
{
    int n = (int)strlen(word);
    if(strncmp(line,word,n) != 0)
        return 0;

    if(line[n] == '\0' || isspace((unsigned char)line[n]) || line[n] == ',' )
        return 1;

    return 0;
}

/*
 * Returns a pointer to the operand part of an instruction.
 * It skips the mnemonic, spaces, tabs, and optional comma separators.
 */
char *getOperand(char *line,char *mnemonic)
{
    char *p = line + strlen(mnemonic);
    while(*p == ' ' || *p == '\t' || *p == ',')
        p++;
    return p;
}

/*
 * Converts a numeric token into an integer value.
 * Supports hexadecimal ending with H, decimal ending with D, binary ending with B, and 0X-style hexadecimal.
 */
int parseNumber(char *token,int *value)
{
    char buf[60];
    char *endptr;
    int base = 10;
    int n;

    strncpy(buf,token,sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    trim(buf);

    n = (int)strlen(buf);
    if(n == 0)
        return 0;

    if(n > 1 && buf[n-1] == 'H') {
        base = 16;
        buf[n-1] = '\0';
    }
    else if(n > 1 && buf[n-1] == 'D') {
        base = 10;
        buf[n-1] = '\0';
    }
    else if(n > 1 && buf[n-1] == 'B') {
        base = 2;
        buf[n-1] = '\0';
    }
    else if(n > 2 && buf[0] == '0' && buf[1] == 'X') {
        base = 16;
    }

    *value = (int)strtol(buf,&endptr,base);
    if(*endptr != '\0')
        return 0;

    return 1;
}

/*
 * Parses an operand as either a number or a label.
 * If the token is a known label, its address is returned through value.
 */
int parseValueOrLabel(char *token,int *value)
{
    int addr;
    char buf[60];

    strncpy(buf,token,sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    trim(buf);

    if(parseNumber(buf,value))
        return 1;

    addr = findLabel(buf);
    if(addr != -1) {
        *value = addr;
        return 1;
    }

    return 0;
}

/*
 * Checks whether the current line matches a specific instruction mnemonic.
 * It ensures the mnemonic match is complete and not just a partial prefix match.
 */
int matchInstruction(char *line,char *mnemonic)
{
    int n = (int)strlen(mnemonic);

    if(strncmp(line,mnemonic,n) != 0)
        return 0;

    if(line[n] == '\0' || line[n] == ' ' || line[n] == '\t' || line[n] == ',')
        return 1;

    return 0;
}

/*
 * Prints one machine-code byte with the current 8085 memory address.
 * After printing, the address counter is incremented by one.
 */
void printByte(int byte)
{
    printf("%04XH:    %02X\n",address & 0xFFFF,byte & 0xFF);
    address = (address + 1) & 0xFFFF;
}

/*
 * Converts 1-byte 8085 instructions into machine code.
 * It searches the 1-byte instruction lookup table and prints the matched opcode.
 */
void getMachineCode_1(char *ptr,int length)
{
    int i,flag=0;
    (void)length;

    for(i=0;i<lut1_count;i++)
    {
        if(strcmp(ptr,lut1[i].mnemonic)==0) {
                flag = 1;
                break;
        }
    }
    if(flag==1){
         int opcode;
         opcode = (int)strtol(lut1[i].opcode,NULL,16);
         printByte(opcode);
         f = 1;
    }
}

/*
 * Converts 2-byte 8085 instructions into machine code.
 * Format: opcode followed by one 8-bit immediate data or port value.
 */
void getMachineCode_2(char *ptr,int length)
{
    int i,flag=0;
    int data,opcode;
    char operand[60];
    (void)length;

    for(i=0;i<lut3_count;i++){
        if(matchInstruction(ptr,lut3[i].mnemonic))
        {
             strcpy(operand,getOperand(ptr,lut3[i].mnemonic));
             trim(operand);

             if(parseValueOrLabel(operand,&data)==0)
                 return;

             if(data < 0 || data > 0xFF)
                 return;

             flag = 1;
             break;
        }
    }
    if(flag==1){
        opcode = (int)strtol(lut3[i].opcode,NULL,16);
        printByte(opcode);
        printByte(data);
        f = 1;
    }
}

/*
 * Converts 3-byte 8085 instructions into machine code.
 * Format: opcode followed by 16-bit address/data in little-endian order.
 */
void getMachineCode_3(char *ptr,int length)
{
    int i,flag=0;
    int data,opcode;
    char operand[60];
    (void)length;

    for(i=0;i<lut5_count;i++){
        if(matchInstruction(ptr,lut5[i].mnemonic))
        {
             strcpy(operand,getOperand(ptr,lut5[i].mnemonic));
             trim(operand);

             if(parseValueOrLabel(operand,&data)==0)
                 return;

             if(data < 0 || data > 0xFFFF)
                 return;

             flag = 1;
             break;
        }
    }
    if(flag==1){
        opcode = (int)strtol(lut5[i].opcode,NULL,16);
        printByte(opcode);
        printByte(data & 0xFF);          //8085 stores 16-bit address as lower byte first
        printByte((data >> 8) & 0xFF);
        f = 1;
    }
}

/*
 * Counts how many bytes will be generated by a DB directive.
 * Character strings are counted based on their character length.
 */
int countDBItems(char *operand)
{
    int count = 0;
    char temp[MAX_LINE_LEN];
    char *tok;

    strncpy(temp,operand,sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';

    tok = strtok(temp,",");
    while(tok != NULL) {
        trim(tok);
        if(tok[0] == '\'' || tok[0] == '"') {
            int l = (int)strlen(tok);
            if(l >= 2)
                count += l - 2;
        }
        else {
            count++;
        }
        tok = strtok(NULL,",");
    }
    return count;
}

/*
 * Emits bytes for the DB directive.
 * Supports numeric byte values and quoted character/string data.
 */
void emitDB(char *operand)
{
    char temp[MAX_LINE_LEN];
    char *tok;
    int value;
    int i,l;

    strncpy(temp,operand,sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';

    tok = strtok(temp,",");
    while(tok != NULL) {
        trim(tok);
        l = (int)strlen(tok);
        if(l >= 2 && (tok[0] == '\'' || tok[0] == '"')) {
            for(i=1;i<l-1;i++)
                printByte((unsigned char)tok[i]);
        }
        else if(parseValueOrLabel(tok,&value) && value >= 0 && value <= 0xFF) {
            printByte(value);
        }
        else {
            printf("INVALID DB DATA: %s\n",tok);
            f = 0;
            return;
        }
        tok = strtok(NULL,",");
    }
    f = 1;
}

/*
 * Emits word data for the DW directive.
 * Each 16-bit word is printed in 8085 little-endian format: lower byte first, higher byte second.
 */
void emitDW(char *operand)
{
    char temp[MAX_LINE_LEN];
    char *tok;
    int value;

    strncpy(temp,operand,sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';

    tok = strtok(temp,",");
    while(tok != NULL) {
        trim(tok);
        if(parseValueOrLabel(tok,&value) && value >= 0 && value <= 0xFFFF) {
            printByte(value & 0xFF);
            printByte((value >> 8) & 0xFF);
        }
        else {
            printf("INVALID DW DATA: %s\n",tok);
            f = 0;
            return;
        }
        tok = strtok(NULL,",");
    }
    f = 1;
}

/*
 * Calculates how many bytes an instruction or directive will occupy.
 * This is mainly used during pass one to calculate label addresses correctly.
 */
int instructionSize(char *line)
{
    int i;
    int value;
    char operand[MAX_LINE_LEN];

    if(line[0] == '\0')
        return 0;

    if(startsWithWord(line,"ORG"))
        return 0;

    if(startsWithWord(line,"DB")) {
        strcpy(operand,getOperand(line,"DB"));
        return countDBItems(operand);
    }

    if(startsWithWord(line,"DW")) {
        strcpy(operand,getOperand(line,"DW"));
        return countDBItems(operand) * 2;
    }

    if(startsWithWord(line,"DS")) {
        strcpy(operand,getOperand(line,"DS"));
        if(parseNumber(operand,&value) && value >= 0)
            return value;
        return 0;
    }

    for(i=0;i<lut1_count;i++) {
        if(strcmp(line,lut1[i].mnemonic)==0)
            return 1;
    }

    for(i=0;i<lut3_count;i++) {
        if(matchInstruction(line,lut3[i].mnemonic))
            return 2;
    }

    for(i=0;i<lut5_count;i++) {
        if(matchInstruction(line,lut5[i].mnemonic))
            return 3;
    }

    return 0;
}

/*
 * First pass of the assembler.
 * It scans all input lines, handles ORG, calculates addresses, and builds the label table.
 */
void passOne(void)
{
    int i;
    int currentAddress = 0;
    char line[MAX_LINE_LEN];
    char operand[MAX_LINE_LEN];
    int orgValue;

    label_count = 0;

    for(i=0;i<len;i++) {
        strcpy(line,str[i]);
        cleanLine(line);
        removeLabelFromLine(line,currentAddress);

        if(line[0] == '\0')
            continue;

        if(startsWithWord(line,"ORG")) {
            strcpy(operand,getOperand(line,"ORG"));
            if(parseNumber(operand,&orgValue))
                currentAddress = orgValue & 0xFFFF;
            continue;
        }

        currentAddress = (currentAddress + instructionSize(line)) & 0xFFFF;
    }
}

/*
 * Program entry point.
 * Reads assembly input, performs pass one for labels, and then performs pass two to generate machine code.
 */
int main()
{
    char temp[MAX_LINE_LEN];
    char line[MAX_LINE_LEN];
    char operand[MAX_LINE_LEN];
    int i;
    int tmp;
    int value;

    printf("ENTER THE 8085 ASSEMBLY CODE:\n\n");
    printf("For e.g\nMOV x,y\nSBB x\nMVI x,96H\n\n");
    printf("Originate the address using ORG, for example ORG 0000H\n\n");

    while(1)
    {
       if(fgets(temp,sizeof(temp),stdin)==NULL)
            break;

       tmp = (int)strlen(temp);
       if(tmp > 0 && temp[tmp-1] == '\n') {
            temp[tmp-1] = '\0';
            tmp--;
       }

       strcpy(line,temp);
       cleanLine(line);

       if(line[0] == '\0')
            continue;

       if(strcmp(line,"END")==0)
            break;
       else {
            if(len >= MAX_LINES) {
                printf("MAXIMUM ASSEMBLY CODE LINE LIMIT REACHED!!\n");
                break;
            }
            strcpy(str[len],temp);
            size[len++] = tmp;
        }
    }

    passOne();

    printf("\nEquivalent machine code is:\n\n");
    address = 0;

    for(i=0;i<len;i++)
    {
         strcpy(line,str[i]);
         cleanLine(line);
         removeLabelFromLine(line,address);

         if(line[0] == '\0')
            continue;

         f = 0;

         if(startsWithWord(line,"ORG")) {
            strcpy(operand,getOperand(line,"ORG"));
            if(parseNumber(operand,&value)) {
                address = value & 0xFFFF;
                f = 1;
            }
         }
         else if(startsWithWord(line,"DB")) {
            strcpy(operand,getOperand(line,"DB"));
            emitDB(operand);
         }
         else if(startsWithWord(line,"DW")) {
            strcpy(operand,getOperand(line,"DW"));
            emitDW(operand);
         }
         else if(startsWithWord(line,"DS")) {
            strcpy(operand,getOperand(line,"DS"));
            if(parseNumber(operand,&value) && value >= 0) {
                address = (address + value) & 0xFFFF;
                f = 1;
            }
         }
         else {
            getMachineCode_1(line,(int)strlen(line));
            if(f==0) getMachineCode_2(line,(int)strlen(line));
            else if(f==1) continue;
            if(f==0)  getMachineCode_3(line,(int)strlen(line));
            else if(f==1) continue;
         }

         if(f==0){
               printf("ENTER VALID ASSEMBLY CODE!! Error near line: %s\n",str[i]);
               break;
        }
    }
}

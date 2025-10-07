/**********************************8085 SIMULATOR************************************/

/*NOTE_1: correctly update the flags for each instructions
    NOTE_2: If a register pair is storing the address means (e.g. B and C) MSB is stored in B and LSB is stored in C
                      e.g if address is 800AH means B = 80, C = 0A
    NOTE_3: 8085 simulator has 16 bit of address bus so memory can be indicated by 2 bytes
    NOTE_4: Enter all the assembly code in UPPERCASE only, also don't leave space unnecessarily
    NOTE_5: Follow the LITTLE ENDIANESS while storing in memory
    NOTE_6: Use address as label during CALL and JUMP instructions
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
char memory[65536][15];                            //storing the memory contents,each of one byte
int addr;                                                            //index value for each memory location,starting address of machine code address
unsigned int A=0x12,B,C,D,E,H=0xAB,L=0x12,SP=0x5000;          //registers present in the 8085, stack pointer at 0x5000
unsigned int ZF,SF,PF,CF,AC;                             //initially all flag bits are get reset to 0
unsigned int flagRegister;                                         //containing all flag bits
unsigned int PC;                                                           //program counter

char str[100][20],f=0;
int size[100];
int len,len1;
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
                                         "BF","B8","B9","BA","BB","BC","BD","BD",
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

char isPresent(char *s1,char *s2)          //check whether s1 is present in s2
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
void pop()
{
       unsigned int dec2 = strtol(memory[SP],NULL,16);
       SP++;
       unsigned int tL = dec2&0x00FF;
       dec2 = strtol(memory[SP],NULL,16);
       SP++;
       unsigned int tH = (dec2<<8)&0xFF00;
       PC = tH|tL;                                         //copying stack top address to PC
}
void swap(unsigned int *a,unsigned int *b)
{
     unsigned int tmp;
     tmp = *a;
     *a = *b;
     *b = tmp;
}
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
void calculateAuxilaryCarry_sub(unsigned int a,unsigned int b)
{
    unsigned char nibble1 = a&0x0F;
    unsigned char nibble2 = b&0x0F;
    unsigned char nibbleSum = nibble1 - nibble2;
    AC = (nibbleSum&0x10)?1:0;
}
void calculateAuxilaryCarry(unsigned int a,unsigned int b)    //function for checking auxilary carry
{
    unsigned char nibble1 = a&0x0F;
    unsigned char nibble2 = b&0x0F;
    unsigned char nibbleSum = nibble1 + nibble2;
    AC = (nibbleSum&0x10)?1:0;
}
void flagStatusUpdate(unsigned int a)   //int size is 4 bytes(e.g  0x1234)
{
    if((a&0x0100)==0x0100) CF = 1;
    else CF = 0;
    if((a&0x80)==0x80) SF = 1;
    else SF = 0;
    if(a==0) ZF = 1;
    else ZF = 0;
    if(checkParity(a)==1) PF = 1;
    else PF = 0;
    flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
}
void DCRflagStatusUpdate(unsigned int a)    //except carry flag all will get affected
{                                                                                       //used for updating flags during INR and DCR operations
    if((a&0x80)==0x80) SF = 1;
    else SF = 0;
    if(a==0) ZF = 1;
    else ZF = 0;
    if(checkParity(a)==1) PF = 1;
    else PF = 0;
    flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
}
void CMPflagStatusUpdate(unsigned int k)           //used only for CMP instruction
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
void getMachineCode_1(char *ptr,int length)
{
    char s1[5],s2[5],c=1;
    int i,flag=0;
    for(i=0;i<200;i++)
    {
        if(strcmp(ptr,lut1[i])==0) {
                flag = 1;
                break;
        }
    }
    if(flag==1){
         printf("%s\n",lut2[i]);
         strcpy(memory[addr++],lut2[i]);
         f = 1;
    }
    else if(flag==0) return;
}
void getMachineCode_2(char *ptr,int length)
{
    char str1[3],i,flag=0,c=1;
    char s1[5],s2[7];
    unsigned int temp;
    str1[2] = '\0';
    for(i=0;i<15;i++){
        if(isPresent(lut3[i],ptr))
        {
             str1[1] = ptr[length-2];
             str1[0] = ptr[length-3];
             flag = 1;
             break;
        }
    }
    if(flag==1){
        printf("%s  %s\n",lut4[i],str1);
        strcpy(memory[addr++],lut4[i]);
        strcpy(memory[addr++],str1);
        len1++;
        f = 1;
    }
    else if(flag==0) return;
}
void getMachineCode_3(char *ptr,int length)
{
    char str1[3],str2[3],i,flag=0,c=1;
    char s1[7],s2[7];
    unsigned int temp;
    str1[2] = '\0'; str2[2] = '\0';
    for(i=0;i<30;i++){
        if(isPresent(lut5[i],ptr))
        {
             str2[1] = ptr[length-2];
             str2[0] = ptr[length-3];
             str1[1] = ptr[length-4];
             str1[0] = ptr[length-5];
             flag = 1;
             break;
        }
    }
    if(flag==1){
        printf("%s  %s  %s\n",lut6[i],str2,str1);
        strcpy(memory[addr++],lut6[i]);
        strcpy(memory[addr++],str2);
        strcpy(memory[addr++],str1);
        len1 = len1+2;
        f = 1;
    }
    else if(flag==0) return;
}
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
int main()
{
    char temp[20],i,y=0;
    int tmp;
    for(int i=0;i<65536;i++) strcpy(memory[i],"00");
    init_messages();
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nENTER THE VALUES TO BE STORED IN MEMORY LOCATION (enter NIL if not any and enter to END to terminate):\n");
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nFor e.g\n0012H  23\nC301H  1A\n\n");
    while(1)
    {
        char c=1;
        int n1;
        scanf("%[^\n]",temp);
        getchar();
        if(strcmp(temp,"END")==0||strcmp(temp,"NIL")==0) break;
        else{
              char *token = strtok(temp,"  ");
              while(token!=NULL){
                   if(c==1){
                       c++;
                       n1 = strtol(token,NULL,16);
                   }
                   else if(c==2) strcpy(memory[n1],token);
                   token = strtok(NULL,"  ");
              }
        }
    }
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nENTER THE ASSEMBLY CODE (all in upper case only):\n");
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nFor e.g\nMOV x,y\nSBB x\nMVI x,96H\n\n");
    printf("---------------------------------------------------------------------------------------------------------");
    printf("\nSTART WITH ANY ORGINATING ADDRESS (e.g., ORG 2000H) & TERMINATE WITH END\n");
    printf("---------------------------------------------------------------------------------------------------------\n");
    while(1)
    {
        if(y==0){
            y++;
            scanf("ORG %xH",&addr);
            getchar();
        }
        else{
       scanf("%[^\n]%n",temp,&tmp);
       getchar();
       if(strcmp(temp,"END")==0) break;
       else {
            strcpy(str[len],temp);
            size[len++] = tmp;
         }
      }
   }
   len1 = len;
   printf("\n---------------------------------------------------------------------------------------------------------");
   printf("\nEQUIVALENT MACHINE CODE\n");
   printf("---------------------------------------------------------------------------------------------------------\n");
    for(i=0;i<len;i++)
    {
         f = 0;
         getMachineCode_1(str[i],size[i]);
         if(f==0) getMachineCode_2(str[i],size[i]);
         else if(f==1) continue;
         if(f==0)  getMachineCode_3(str[i],size[i]);
         else if(f==1) continue;
         if(f==0){
               printf("ENTER VALID ASSEMBLY CODE!!\n");
               break;
        }
    }
    printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nCODE MEMORY IS SHOWN BELOW\n");
    printf("---------------------------------------------------------------------------------------------------------\n");
    for(int i=addr-len1;i<addr;i++) printf("%04x       %s\n",i,memory[i]);
    PC = addr-len1;
    for(PC;PC<addr;PC++)                                  //starting from the Program counter (PC)
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
         else if(strcmp(memory[PC],"BD")==0) {
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
                D = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"77")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",A);                       //writing back to memory
                strcpy(memory[dec],string);            //M = A
        }
        else if(strcmp(memory[PC],"70")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",B);                       //writing back to memory
                strcpy(memory[dec],string);            //M = B
        }
        else if(strcmp(memory[PC],"71")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",C);                       //writing back to memory
                strcpy(memory[dec],string);            //M = C
        }
        else if(strcmp(memory[PC],"72")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",D);                       //writing back to memory
                strcpy(memory[dec],string);            //M = D
        }
        else if(strcmp(memory[PC],"73")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",E);                       //writing back to memory
                strcpy(memory[dec],string);            //M = E
        }
        else if(strcmp(memory[PC],"74")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",H);                       //writing back to memory
                strcpy(memory[dec],string);            //M = H
        }
        else if(strcmp(memory[PC],"75")==0){
                char string[10];
                sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                int dec = strtol(string,NULL,16);
                sprintf(string,"%x",L);                       //writing back to memory
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
        else if(strcmp(memory[PC],"B7")==0){
                 AC = 0;
                 CF = 0;
                  B = A|B;
                  flagStatusUpdate(B);
                  B = B&0xFF;
        }
        else if(strcmp(memory[PC],"B1")==0){
                  AC = 0;
                  CF = 0;
                  C = A|C;
                  flagStatusUpdate(C);
                  C = C&0xFF;
        }
        else if(strcmp(memory[PC],"B2")==0){
                  AC = 0;
                  CF = 0;
                  D = A|D;
                  flagStatusUpdate(D);
                  D = D&0xFF;
        }
        else if(strcmp(memory[PC],"B3")==0){
                  AC = 0;
                  CF = 0;
                  E = A|E;
                  flagStatusUpdate(E);
                  E = E&0xFF;
        }
        else if(strcmp(memory[PC],"B4")==0){
                  AC = 0;
                  CF = 0;
                  H = A|H;
                  flagStatusUpdate(H);
                  H = H&0xFF;
        }
        else if(strcmp(memory[PC],"B5")==0){
                  AC = 0;
                  CF = 0;
                  L = A|L;
                  flagStatusUpdate(L);
                  L = L&0xFF;
        }
        else if(strcmp(memory[PC],"B6")==0){
                 AC = 0;
                 CF = 0;
                 char string[10];
                 sprintf(string,"%02x%02x",H,L);          //combining two characters as string
                 int dec = strtol(string,NULL,16);          //converting string to hex
                 unsigned int dec2 = strtol(memory[dec],NULL,16);
                 dec2 = A|dec2;
                 flagStatusUpdate(dec2);
                 dec2 = dec2&0xFF;
                 sprintf(string,"%x",dec2);
                 strcpy(memory[dec],string);
        }
        //for POP
        else if(strcmp(memory[PC],"C1")==0){
                 unsigned int dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 B = dec2&0xFF;
                 dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 C = dec2&0xFF;
            }
        else if(strcmp(memory[PC],"D1")==0){
                 unsigned int dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 D = dec2&0xFF;
                 dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 E = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"E1")==0){
                 unsigned int dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 H = dec2&0xFF;
                 dec2 = strtol(memory[SP],NULL,16);
                 SP++;
                 L = dec2&0xFF;
        }
        else if(strcmp(memory[PC],"F1")==0){
                unsigned int dec2 = strtol(memory[SP],NULL,16);
                SP++;
                A = dec2&0xFF;
                dec2 = strtol(memory[SP],NULL,16);
                SP++;
                flagRegister = dec2&0xFF;
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
        else if(strcmp(memory[PC],"9F")==0) {
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
                A = A^C;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AA")==0) {
                A = A^D;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AB")==0) {
                A = A^E;
                flagStatusUpdate(A);
                A = A&0xFF;
        }
        else  if(strcmp(memory[PC],"AC")==0) {
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
             char string[10];
             sprintf(string,"%02x%02x",H,L);          //combining two characters as string, H and L together stores address
             int dec = strtol(string,NULL,16);   //converting string to hex
             unsigned int dec2 = strtol(memory[dec],NULL,16);    //getting data from the memory location(memory[dec])
             A = A^dec2;
             flagStatusUpdate(A);
             A = A&0xFF;
        }
        //for CMA
        else if(strcmp(memory[PC],"2F")==0) A = ~A;
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
            A = (A<<1)+CF;
            if((A&0x0100)==0x0100) CF = 1;
            else CF = 0;
            A = A&0xFF;                                     //since registers only the 8 bits, so we are typecasting to store first 8 bits alone
            flagRegister = (SF<<7)|(ZF<<6)|(AC<<4)|(PF<<2)|(CF);     //updating flag register
       }
       //for RAR
       else if(strcmp(memory[PC],"1F")==0){            //rotate to right
            A = (A>>1)+(CF<<8);
            if((A&0x0100)==0x0100) CF = 1;
            else CF = 0;
            A = A&0xFF;                     //since registers only the 8 bits, so we are typecasting to store first 8 bits alone
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
       //for HLT
       else if(strcmp(memory[PC],"76")==0) break;            //ending the program
       //RET
       else if(strcmp(memory[PC],"C9")==0) pop();            //copying stack top to PC
       //PCHL
       else if(strcmp(memory[PC],"E9")==0) PC = ((H<<8)&0xFF00)|(L&0x00FF);
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
             strcpy(memory[dec],memory[PC]);
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
           sprintf(memory[temp],"%x",L&0xFF);
           temp++;
           sprintf(memory[temp],"%x",H&0xFF);
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
           sprintf(memory[temp],"%x",A&0xFF);
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
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(CF==1)PC = temp-1;
     }
     //CM
     else if(strcmp(memory[PC],"FC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(SF==1)PC = temp-1;
     }
     //CNC
     else if(strcmp(memory[PC],"D4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(CF==0)PC = temp-1;
     }
     //CNZ
     else if(strcmp(memory[PC],"C4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(ZF==0)PC = temp-1;
     }
     //CP
     else if(strcmp(memory[PC],"F4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(SF==0)PC = temp-1;
     }
     //CPE
     else if(strcmp(memory[PC],"EC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(PF==1)PC = temp-1;
     }
     //CPO
     else if(strcmp(memory[PC],"E4")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(PF==0)PC = temp-1;
     }
     //CZ
     else if(strcmp(memory[PC],"CC")==0){
              unsigned int temp;
              char strH[3], strL[3];
              strcpy(strL,memory[PC+1]);
              strcpy(strH,memory[PC+2]);
              PC +=2;
              push(PC+1);               //pushing next address to stack
              strcat(strH,strL);
              sscanf(strH,"%x",&temp);
              if(ZF==1)PC = temp-1;
     }
 }
    printf("\n---------------------------------------------------------------------------------------------------------");
    printf("\nFINAL STATUS OF REGISTERS\n");
    printf("---------------------------------------------------------------------------------------------------------\n");
    printf("A  ---> %02x\n",A); printf("B  ---> %02x\n",B); printf("C  ---> %02x\n",C); printf("D  ---> %02x\n",D);
    printf("E  ---> %02x\n",E); printf("H  ---> %02x\n",H); printf("L  ---> %02x\n",L); printf("SP  ---> %02x\n",SP);
    printf("PC  ---> %04x\n",PC);
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
         int h;
         scanf("%04xH",&h);
         if(h==-1) break;
         else printf("%04x      %s\n",h,memory[h]);
    }
}




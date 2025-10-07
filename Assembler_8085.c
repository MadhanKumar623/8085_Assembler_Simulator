//8085 assembler has 16 bit of address bus so memory can be indicated by 2 bytes
#include<stdio.h>
#include<string.h>
char str[100][20],f=0;
int size[100];
int addr_array[20],m=-1;               //stack storing the address
int len,address;
char lut1[200][10] = {  "ADC A","ADC B","ADC C","ADC D","ADC E","ADC H","ADC L","ADC M",
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
                                         "RST 0","RST 1","RST 2","RST 3","RST 4","RST 5","RST 6","RST 7",
                                         "SBB A","SBB B","SBB C","SBB D","SBB E","SBB H","SBB L","SBB M",
                                         "SUB A","SUB B","SUB C","SUB D","SUB E","SUB H","SUB L","SUB M",
                                         "STAX B","STAX D",
                                         "XRA A","XRA B","XRA C","XRA D","XRA E","XRA H","XRA L","XRA M",
                                         "HLT","NOP","PCHL","CMA","CMC","DAA","DI","EI",
                                         "RAL","RAR","RC","RET","RIM","RLC","RM","RNC","RNZ","RP","RPE","RPO","RRC",
                                         "RZ","SIM","SPHL","STC","XCHG","XTHL"
                                         };
char lut2[200][7] = {"8F","88","89","8A","8B","8C","8D","8E",
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
char lut3[16][7] =   { "ACI","ADI","ANI",
                                       "MVI A","MVI B","MVI C","MVI D","MVI E","MVI H","MVI L","MVI M",
                                       "ORI","SBI","SUI","XRI"
                                     };
char lut4[16][4] =  {  "CE","C6","E6",
                                       "3E","06","0E","16","1E","26","2E","36",
                                       "F6","DE","D6","EE"
                                    };
char lut5[30][7] = {"IN","LDA","LHLD","OUT","SHLD","STA","LXI B","LXI D","LXI H","LXI SP",
                                    "CALL","CC","CM","CNC","CNZ","CP","CPE",
                                    "CPI","CPO","CZ","JC","JM","JMP","JNC","JNZ",
                                    "JP","JPE","JPO","JZ"
                                   };
char lut6[30][7] = {"DB","3A","2A","D3","22","32","01","11","21","31",
                                     "CD","DC","FC","D4","C4","F4","EC",
                                    "FE","E4","CC","DA","FA","C3","D2","C2",
                                    "F2","EA","E2","CA"
                                   };

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
void getMachineCode_1(char *ptr,int length)
{
    int i,flag=0;
    for(i=0;i<200;i++)
    {
        if(strcmp(ptr,lut1[i])==0) {
                flag = 1;
                break;
        }
    }
    if(flag==1){
         printf("%04xH:    ",address++);
         printf("%s\n",lut2[i]);
         f = 1;
    }
}
void getMachineCode_2(char *ptr,int length)
{
    char str1[3],i,flag=0;
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
        printf("%04xH:    ",address++);
        printf("%s\n",lut4[i]);
        printf("%04xH:    ",address++);
        printf("%s\n",str1);
        f = 1;
    }
}
void getMachineCode_3(char *ptr,int length)
{
    char str1[3],str2[3],i,flag=0;
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
        printf("%04xH:    ",address++);
        printf("%s\n",lut6[i]);
        printf("%04xH:    ",address++);
        printf("%s\n",str2);
        printf("%04xH:    ",address++);
        printf("%s\n",str1);
        f = 1;
    }
}
int main()
{
    char temp[20],i;
    int tmp;
    printf("ENTER THE ASSEMBLY CODE (all in upper case only):\n\n");
    printf("For e.g\nMOV x,y\nSBB x\nMVI x,96H\n\n");
    printf("Originate the address from 0000H\n\n");
    while(1)
    {
       scanf("%[^\n]%n",temp,&tmp);
       getchar();
       if(strcmp(temp,"ORG 0000H")==0) continue;
       else if(strcmp(temp,"END")==0) break;
       else {
            strcpy(str[len],temp);
            size[len++] = tmp;
        }
    }
    printf("\nEquivalent machine code is:\n\n");
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
}




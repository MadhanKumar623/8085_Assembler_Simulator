# 8085 Assembler in C

This project is a simple **Intel 8085 assembler written in C**.

It started from an older assembler program that used lookup tables to convert 8085 assembly instructions into machine code. The revised version keeps the same basic logic and structure, but adds better instruction support, labels, comments, `ORG`, and data directives.

---

## Project Versions

### 1. Initial Code

The initial code used three main machine-code generation functions:

```c
getMachineCode_1();   // 1-byte instructions
getMachineCode_2();   // 2-byte instructions
getMachineCode_3();   // 3-byte instructions
```

It worked for many basic 8085 instructions, but had limitations:

- only handled `ORG 0000H`
- no label support
- no comments
- no `DB`, `DW`, or `DS`
- some instruction forms were missing
- some similar mnemonics could cause matching issues

### 2. Revised Code

The revised code keeps the same lookup-table based design, but adds:

- more complete 8085 instruction support
- labels and forward labels
- custom `ORG` address
- comments using `;` or `//`
- lowercase input support
- `DB`, `DW`, and `DS` directives
- better parsing for numbers and labels

---

## Build

Compile using GCC:

```bash
gcc -std=c99 -Wall -Wextra -Wpedantic 8085_assembler.c -o 8085_assembler
```

Run:

```bash
./8085_assembler
```

On Windows:

```bash
gcc 8085_assembler.c -o 8085_assembler.exe
8085_assembler.exe
```

---

## Supported Syntax

### Basic Instructions

```asm
MOV A,B
MVI A,25H
ADD B
STA 2050H
JMP 1000H
HLT
```

### Labels

```asm
START: MVI A,05H
LOOP:  DCR A
       JNZ LOOP
       HLT
```

### ORG

```asm
ORG 2000H
MVI A,55H
HLT
END
```

### Data Directives

```asm
VALUE: DB 25H
ADDR:  DW 2050H
BUF:   DS 04H
```

---

## Example Program

```asm
ORG 0000H
MVI A,25H
MVI B,05H
ADD B
STA 2050H
HLT
END
```

Expected bytes:

```text
3E 25 06 05 80 32 50 20 76
```

Address-wise output:

```text
0000H: 3E
0001H: 25
0002H: 06
0003H: 05
0004H: 80
0005H: 32
0006H: 50
0007H: 20
0008H: 76
```

---

## Edge Case Test

```asm
ORG 0000H

JM 1234H
JMP 1234H
JP 1234H
JPE 1234H
JPO 1234H

HLT
END
```

Expected bytes:

```text
FA 34 12
C3 34 12
F2 34 12
EA 34 12
E2 34 12
76
```

This checks that similar instructions like `JM`, `JMP`, `JP`, `JPE`, and `JPO` are matched correctly.

---

## Known Limitations

This is still a learning/project-level assembler, not a fully production-grade assembler.

Current limitations:

- output is printed to console only
- no Intel HEX or `.bin` file generation
- no macro support
- no `EQU` support
- no expression handling like `LABEL + 1`
- duplicate label handling can be improved
- address overflow checking can be improved

---

## Future Improvements

Possible next steps:

- add file input/output
- generate Intel HEX file
- generate binary file
- add better error messages with line numbers
- add duplicate label error detection
- add macro and `EQU` support
- add automated test cases

---

## Summary

The revised assembler keeps the original logic:

```text
assembly instruction -> lookup table -> opcode -> machine code
```

The main improvement is that it adds the common assembler features needed to write and test more realistic 8085 assembly programs.

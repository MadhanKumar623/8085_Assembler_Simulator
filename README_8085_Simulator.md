# 8085 Simulator

A simple C-based 8085 simulator and assembler. It reads 8085 assembly instructions, converts them into machine code, stores them in simulated 64 KB memory, executes the code, and prints the final register and flag status.

This README applies to:

- the original 8085 simulator code
- the revised simulator code with improved input handling and bug fixes

## Features

### Original Code

- Supports many 8085 instructions such as `MOV`, `MVI`, `ADD`, `SUB`, `ADC`, `SBB`, `INR`, `DCR`, `LXI`, `LDA`, `STA`, `JMP`, `CALL`, `RET`, `PUSH`, `POP`, and `HLT`.
- Uses a 64 KB memory array to simulate the 8085 address space.
- Converts assembly instructions into machine code using lookup tables.
- Executes the generated machine code and displays final register and flag values.

### Revised Code

- Keeps the original API/function structure as much as possible.
- Adds short comments above APIs/functions.
- Handles extra spaces, lowercase input, comments, labels, and constants.
- Supports label-based jumps/calls like `JNZ LOOP` and `CALL DELAY`.
- Supports `DB` data directive and `EQU` constants.
- Fixes opcode and execution issues such as `CMP M`, `SUB A`, `MOV L,M`, `ORA`, stack operations, `CALL/RET`, and `MVI M`.
- Adds better memory byte formatting and basic `IN`/`OUT` port simulation.

## Build

Use GCC:

```bash
gcc -std=c11 -Wall -Wextra -pedantic revised_8085_simulator.c -o sim8085
```

Run:

```bash
./sim8085
```

On Windows:

```bash
gcc -std=c11 -Wall -Wextra -pedantic revised_8085_simulator.c -o sim8085.exe
sim8085.exe
```

## Input Format

The simulator first asks whether any memory locations should be preloaded.

Use `NIL` when no memory preload is required:

```text
NIL
```

Or preload memory like this:

```text
3000H 55H
3001H AAH
END
```

Then enter the assembly program. Start with `ORG` and finish with `END`:

```asm
ORG 2000H
MVI B,03H
MVI A,00H
LOOP: ADI 01H
DCR B
JNZ LOOP
HLT
END
```

After execution, enter memory addresses to inspect. Use `-1` to stop:

```text
2000H
3000H
-1
```

## Example Program

```asm
ORG 2000H
MVI B,03H
MVI A,00H
LOOP: ADI 01H
DCR B
JNZ LOOP
HLT
END
```

Expected final result:

```text
A = 03H
B = 00H
ZF = 1
SF = 0
PF = 1
CF = 0
```

## Notes

- `NIL` is not an 8085 instruction. It is only used by this simulator to skip memory preload.
- The simulator follows little-endian storage for 16-bit operands.
- The original code initializes some registers with custom values, such as `H = ABH`, `L = 12H`, and `SP = 5000H`. This can differ from online simulators like Sim8085.
- PC output may differ between simulators. Some show the address of `HLT`; others show the next address after `HLT`.

## File

Main source file:

```text
revised_8085_simulator.c
```

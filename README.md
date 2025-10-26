# Two-Pass Assembler (Final Project: System Programming Lab)

## Project Overview

This project is a complete implementation of a two-pass assembler written in the C language. It was developed as the final project for Course System Programming Lab, focusing on core concepts of system programming, memory management, and code translation.

The main purpose of the assembler is to translate source files written in a specific Assembly language into corresponding machine code files. The assembler handles the full process of translation, including:

  * Macro expansion (pre-processing).
  * Symbol management via a dedicated symbol table.
  * Generating relocatable object code.

## Assembler Functionality and Structure

The assembly process is strictly implemented using two distinct passes:

| Pass | Responsibility |
| :--- | :--- |
| **First Pass** | Responsible for pre-processing (macro expansion) and building the symbol table. Initializes and updates the Instruction Counter (IC) and Data Counter (DC). |
| **Second Pass** | Handles the final translation of instructions, resolves addresses using the symbol table, and produces the output files. |

### Macro Support

The assembler supports macro definitions and expansion. Macros are defined using the start directive **`mcro`** and the end directive **`mcroend`**.

## Target Machine Architecture and Constraints

The assembler targets a hypothetical processor defined by the following characteristics:

  * **Machine Word Length:** 14 bits.
  * **Registers:** 8 general-purpose registers (r0 through r7).
  * **Memory:** Both the Instruction Counter (IC) and Data Counter (DC) start counting from memory address **100** (decimal).
  * **Program Status Word (PSW):** The system includes a Program Status Word, with the **Z (Zero) flag** being part of the PSW.
  * **Input Line Limit:** Each line in the source file is limited to a maximum of **80 characters** (including the newline character, `\n`).

### Instruction Set and Addressing Modes

The instruction format supports the following operand configurations:

| Opcode Range | Operands | Example Instructions |
| :---: | :---: | :--- |
| 1–4 | Two | `mov`, `cmp`, `add`, `sub`, `lea` |
| 5–13 | One | `clr`, `not`, `inc`, `dec`, `jmp`, `bne`, `jsr`, `red`, `prn` |
| 14–16 | Zero | `stop` |

The assembler supports the following addressing modes:

  * **Immediate:** The operand is a constant value (e.g., `#7`).
  * **Direct:** The operand is a label (symbol).
  * **Register Direct:** The operand is one of the general-purpose registers (`r0`–`r7`).
  * **Indexed:** Supported through indexing features (fields 6–9 in the instruction word).

### Pseudo-Instructions (Directives)

The following assembler directives are supported:

  * **`.data`**: Used to define a list of numerical data.
  * **`.string`**: Used to define a character string which is terminated by the null character (`\0`, represented as 0).
  * **`.mat`**: Used to define an array or matrix (e.g., `MAT8: .mat`).
  * **`.entry`**: Declares a label defined within the current source file as an entry point for other files.
  * **`.extern`**: Declares a label used in the current source file but defined externally in another file.

## Compilation and Execution

### Compilation Requirements

The C source code **must** be compiled using `gcc` with strict compilation flags:

```bash
gcc -Wall -ansi -pedantic <source_files>
```

### Execution and Output Files

The assembler takes a source file (e.g., `prog.as`) and produces up to three output files based on the base filename.

To run the assembler, provide the **base filename** (without the `.as` extension) as an argument:

```bash
./assembler prog
```

(Assuming the executable is named `assembler`.)

| Output File | Extension | Description |
| :--- | :--- | :--- |
| Object Code | **`.ob`** | Contains the translated machine code, preceded by the final counts of the Instruction Counter (**ICF**) and Data Counter (**DCF**). Each machine word includes the **A, R, E** fields (Absolute, Relocatable, External) for linkage information. |
| Entry Points | **`.ent`** | Lists all symbols explicitly declared using the `.entry` directive and their final address. |
| Externals | **`.ext`** | Lists all external symbols (declared using `.extern`) that were used in the file, along with the memory address where they were referenced. |

### Error Handling

The assembler provides detailed error reporting. If errors are detected during the assembly process, relevant error messages are printed, and the output files (e.g., `.ob`, `.ent`, `.ext`) are **not generated**.
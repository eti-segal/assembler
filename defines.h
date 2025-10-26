#ifndef DEFINES_H
#define DEFINES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

/*for all function use*/
#define ERROR 1
#define TRUE 1
#define FALSE 0

/*for "num of lines" addressing modes */
#define IMMEDIATE_ADD 0
#define DIRECT_ADD 1
#define MATRIX_ADD 2
#define REGISTER_ADD 3
#define NO_OPERAND -1

/*for "line ends here" function*/
#define LINE_DOSENT_END_HERE -1
#define LINE_ENDS_HERE 1

/*if entry/exter lables*/
#define ENTRY_INDEX 3
#define EXTERN_INDEX 4

/*maxes*/
#define MAX_LINES 256
#define MAX_NUM_10B 511
#define MIN_NUM_10B -512


/*sizes*/
#define MAX_LINE 82/*length of a standard line + \n + \0*/
#define LINE_LENGTH 10
#define BITS_OF_DATA_WORD 10
#define MAX_LABEL 31
#define MAX_BINARY_WORD 56
#define WORD_SIZE 11/*with /n */
#define REG_SIZE 3
#define NUMBER_SIZE 8
#define OPERAND_SIZE 3
#define OPCODE_SIZE 5
#define MAX_NAME 20
#define EXTENTION 4

#define DATA_QUANTITY 5
#define INSTRUCTION_QUANTITY 16

#define MIN_ARGC 2

#define BITS_OF_ADDRESS 8
#define ROWS 4
#define COLS 2
#define BITS_TO_CONVERT 2

#endif

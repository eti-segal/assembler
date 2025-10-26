#ifndef TABLES_H
#define TABLES_H

typedef struct 
{
    char symbol_name[MAX_LABEL];
    int address;
    char type[MAX_LABEL]; /*is symbol before data or code*/
}symbols;


typedef struct {
    char name[MAX_LINE];
    long start;
    long end;
} macro;


typedef struct 
{
    char name[MAX_LABEL];
    int ic;
    int line_num;
}use_label;


typedef struct instruction {
    char* name;
    int operand_num;
    int opcode;
    char * opcode_binary;
} instruction;


typedef struct 
{
    char* name;
    int (*func) (int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table);
}data;




#endif

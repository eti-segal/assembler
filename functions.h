#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/*main functions*/
int macro_deployment(int argc, char* file_name, char * am_file_name, data * data_table, instruction * instruction_table);
int first_pass(char * file_name, symbols ** symbols_table, data * data_table, instruction * instruction_table, use_label use_label_table[], char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL]);
int second_pass(char * fp_file_name, symbols * symbols_table, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], use_label use_label_table[]);

/*macro deployment functions*/
/*The find_macro function searches for a macro with the given word as its name in the macro_array, and returns its index if found, or INT_MIN if not*/
int find_macro(macro* macro_array, char * word, int current_array_size);
/*the function moves the cursor to the beginning of the next line*/
void next_line(FILE * file);

/*instruction_function functs*/
char* zero_operand_funct(char * line, int line_num ,instruction inst, use_label use_label_table[]);
char* one_operand_funct(char * line, int line_num ,instruction inst, use_label use_label_table[]);
char * two_operand_funct(char * line, int line_num ,instruction inst, use_label use_label_table[]);

/*data_function functs*/
int data_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table);
int string_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table);
int mat_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table);
int entry_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table);
int extern_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table);

/*first_pass_function functs*/
int correct_commas(char * rest_of_line, int line_num);
int is_data(char * word);
int is_instruction(char * word, instruction * instruction_table);
char * matrix_treatment(int line_num, char * rest_of_line, FILE * file_d, char * type);
int line_ends_here(char * rest_of_line);
int is_valid_number(char * word, int line_num, int min_num, int max_num);/*the function checks if the string is a valid number*/
int is_register(char * word);
char * reg_to_word(int reg_1, int reg_2);
int immediate(char * operand, int line_num);
int label_syntax(char * word, int line_num);
int num_of_lines(int first_operand, int second_operand, int operand_num);
char * is_correct_string(char * word, int line_num);
int find_symbol(char * word, symbols * symbols_table, int current_symbol_count);
int is_mat(char* operand) ;
int check_memory_full(int line_num);
void combine_files(FILE * file_i, FILE * file_d);

/*second pass functions*/
int in_symbol_table(char * label, symbols * symbols_table);
int in_entry_table(char * label, char entry_table[][MAX_LABEL]);
int in_extern_table(char * label, char extern_table[][MAX_LABEL]);
int in_use_table(int ic_current, use_label use_label_table[]);
void update_address(FILE * file_fp, int new_address);
void file_to_special_binary(FILE * file_fp, FILE * file_write);
char * to_special_binary(char * str, int length);
char * delete_leadings(char * str);

/*multi-function functions*/
/* function appends a file extension type to the end of file_name and returns the resulting new string*/
char * replace_extention(char * file_name, char* type);
char * to_binary(long number, int length);/*the function accepts a decimal number and converts it to a binary string of the received length*/


#endif

#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

int data_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table)
{
    if(correct_commas(rest_of_line, line_num))/*the command punctuation is correct*/
    {
        char * word;
        char * binary_num;
        int number;
        word=strtok(rest_of_line, " ,\t\n");/*word reception*/
        if(word==NULL)/*no numbers received*/
        {
            printf("error: line %d no numbers were received in '.data' prompt\n", line_num);
            error=TRUE;
            return FALSE;
        }
        else
        {
            while(word!=NULL)
            {
                if((number=is_valid_number(word, line_num, MIN_NUM_10B, MAX_NUM_10B))==INT_MIN)/*the number isn't valid*/
                {
                    break;
                }
                else/*the number is valid*/
                {
                    if(check_memory_full(line_num)==FALSE)
                    {
                        exit_flag=TRUE;
                        return FALSE;
                    }
                    binary_num = to_binary(number, BITS_OF_DATA_WORD);/*convert to binary*/
                    if(exit_flag)
                    {
                        free(binary_num);
                        return FALSE;
                    }
                    fprintf(file_d, "%s\n",binary_num);/*inserting the number into the data file*/
                    free(binary_num);
                    dc++;/*data counter promotion*/
                }
                word=strtok(NULL, " ,\t\n");/*get the next word*/
            }
        }
    }
    return TRUE;
}

int string_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table)
{
    char * str_content;
    char * binary_char;
    int i;
    str_content=is_correct_string(rest_of_line, line_num);
    if(str_content==NULL)/*the received argument isn't a valid string*/
    {
        return FALSE;
    }
    else/*a valid string*/
    {
        for(i=0; i<strlen(str_content); i++)/*inserting character by character to the data file*/
        {
            if(check_memory_full(line_num)==FALSE)
            {
                exit_flag=TRUE;
                return FALSE;
            }
            binary_char= to_binary((unsigned char)str_content[i], BITS_OF_DATA_WORD);/*convert to binary*/
            if(exit_flag)
            {
                free(binary_char);
                return FALSE;
            }
            fprintf(file_d, "%s\n", binary_char);/*inserting the binary encoding of the character into the data file*/
            free(binary_char);
            dc++;/*data counter promotion*/
        }
        free(str_content);
        if(check_memory_full(line_num)==FALSE)
        {
            exit_flag=TRUE;
            return FALSE;
        }
        fprintf(file_d, "0000000000\n");/*adding a string-end character encoding after character encoding*/
        dc++;/*data counter promotion*/
    }
    return TRUE;
}

int mat_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table)
{
    matrix_treatment(line_num, rest_of_line, file_d, "data");
    return TRUE;
}

/*check if the next word is a valid label, if so - insert the label to the entry_table*/
int entry_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table)
{
    char * word=strtok(rest_of_line, " ,\t\n");/*get the label name*/
    if(((is_data(word))!=INT_MIN) || ((is_instruction(word, instruction_table))!=INT_MIN) || (is_register(word)!=-1))/*the label is name of a command (code or data), or a register*/
    {
        printf("error: file %s line %d the label defined is a reserved name\n",cur_file,  line_num);
        error=TRUE;
        return FALSE;
    }
    if(!label_syntax(word, line_num))/*the label is either syntactically incorrect*/
    {
        error=TRUE;
        return FALSE;
    }
    else/*validte label*/ 
    {
        char * end_of_line=word+strlen(word)+1;/*points to the place after the prompt end*/
        strcpy(entry_table[entries_count], word);/*insert the label name to entry_table*/
        entries_count++;
        if(!line_ends_here(end_of_line))/*checking if too many arguments were received*/
        {
            printf("error: file %s line %d has too many operands\n",cur_file,  line_num);
            error=TRUE;
            return FALSE;
        }
    }
    return TRUE;
}

/*check if the next word is a valid label, if so-insert the label to the extern_table*/
int extern_check(int line_num, char * rest_of_line, FILE * file_d, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], instruction * instruction_table)
{
    char * word=strtok(rest_of_line, " ,\t\n");/*get the label name*/
    if(((is_data(word))!=INT_MIN) || ((is_instruction(word, instruction_table))!=INT_MIN) || (is_register(word)!=-1))/*the label is name of a command (code or data), or a register*/
    {
        printf("error: file %s line %d the label defined is a reserved name\n",cur_file,  line_num);
        error=TRUE;
        return FALSE;
    }
    if(!label_syntax(word, line_num))/*the label is either syntactically incorrect*/
    {
        error=TRUE;
        return FALSE;
    }
    else/*validte label*/ 
    {
        char * end_of_line=word+strlen(word)+1;/*points to the place after the prompt end*/
        strcpy(extern_table[externals_count], word);/*insert the label name to extern_table*/
        externals_count++;
        if(!line_ends_here(end_of_line))/*checking if too many arguments were received*/
        {
            printf("error: file %s line %d has too many operands\n",cur_file,  line_num);
            error=TRUE;
            return FALSE;
        }
    }
    return TRUE;
}


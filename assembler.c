/*Tzivi schleider 327745006
  Eti segal 215020967*/

#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

char cur_file [MAX_NAME+EXTENTION]= {0};
int error=FALSE;/*error flag*/
int ic=99;
int dc=0;/*instructions counter and data counter*/
int current_symbol_count=0;
int entries_count=0;
int externals_count=0;
int use_label_count=0;
int exit_flag=FALSE;

int main(int argc, char * argv[])
{
    int i;
    char * temp_num;/*saves the file name after changing the extension*/

    data data_table[DATA_QUANTITY]={
        {".data", data_check},
        {".string", string_check},
        {".mat", mat_check},
        {".entry", entry_check},
        {".extern", extern_check}
    };/*build data details table*/

    instruction instruction_table[INSTRUCTION_QUANTITY]={
    {"mov", 2, 0, "0000"},
    {"cmp", 2, 1, "0001"},
    {"add", 2, 2, "0010" },
    {"sub", 2, 3, "0011" },
    {"lea", 2, 4, "0100" },
    {"clr", 1, 5, "0101" },
    {"not", 1, 6, "0110" },
    {"inc", 1, 7, "0111" },
    {"dec", 1, 8, "1000" },
    {"jmp", 1, 9, "1001" },
    {"bne", 1, 10, "1010" },
    {"jsr", 1, 11, "1011" },
    {"red", 1, 12, "1100" },
    {"prn", 1, 13, "1101" },
    {"rts", 0, 14, "1110" },
    {"stop", 0, 15, "1111"}
    }; /*matrix to hold instruction names and their details */

    
    
    for(i=1; i<argc; i++)/*goes over all files*/
    {
	char file_name [MAX_NAME+EXTENTION];
        char am_file_name [MAX_NAME+EXTENTION];
        char fp_file_name [MAX_NAME+EXTENTION];
	char entry_table[MAX_LINES][MAX_LABEL]={0};
        char extern_table[MAX_LINES][MAX_LABEL]={0};
        use_label use_label_table[MAX_LINES];
        symbols * symbols_table=(symbols *)malloc(sizeof(symbols));
        ic=99;
        dc=0;
        error=FALSE;/*error flag*/
        current_symbol_count=0;
        entries_count=0;
        externals_count=0;
        use_label_count=0;
        exit_flag=FALSE;
        if (!symbols_table) { perror("malloc failed"); exit(1); }       
        
        if(strlen(argv[i])>MAX_NAME)/*the file name is too long*/
        {
            printf("error: the file name exceeds the maximum size specified\n");
            exit(1);
        }
        temp_num=replace_extention(argv[i], ".as");
        strcpy(file_name, temp_num);/*adds '.as' to the file name in order to see it on*/
        temp_num=replace_extention(file_name, ".am");
        strcpy(cur_file , temp_num);   
        temp_num=replace_extention(file_name, ".am");    
        strcpy(am_file_name, temp_num); 
        temp_num=replace_extention(file_name, ".fp");
        strcpy(fp_file_name, temp_num); 
        free(temp_num);
        macro_deployment(argc, file_name, am_file_name, data_table, instruction_table);/*the file is passed to macro deployment*/ 

        if(error==TRUE)/*there was an error deploying the macro*/
        {
            continue;
        }          
        first_pass(am_file_name, &symbols_table,  data_table, instruction_table,  use_label_table, entry_table, extern_table);

        second_pass(fp_file_name, symbols_table, entry_table, extern_table,  use_label_table);
        
        free(symbols_table);
        symbols_table = NULL;
    }
    return 0;

}




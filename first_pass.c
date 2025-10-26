#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

int first_pass(char * file_name, symbols ** symbols_table, data * data_table, instruction * instruction_table, use_label use_label_table[], char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL])
{
    FILE * file_i=fopen("file_instruction", "w+");
    FILE * file_d=fopen("file_data", "w+");
    FILE * file=fopen(file_name, "r");
    int in_label=FALSE;
    int line_num=0;
    int ins_index, data_index;
    char * rest_of_line;
    char * temp_name;/*saves the file name after changing the extension*/

    /*char word[MAX_LABEL];*/
    char line[MAX_LINE]={0};
    char * word;
    int k;
    int symbol_capacity=1;
    symbols * temp_realloc=NULL; /* temporary pointer for reallocation*/
    if(file==NULL || file_d==NULL || file_i==NULL)
    {
        printf("error PC was unable to open one of the files \n");
	if(file){fclose(file);}
        if(file_i)
	{
		fclose(file_i);
		remove("file_instruction");
	}
        if(file_d)
	{
		fclose(file_d);
		remove("file_data");
	}
        exit(1);
    }
    
    while(fgets(line, MAX_LINE, file)!=NULL)/*as long as we haven't reached the end of the file*/
   {      
        line_num++;
        word=strtok(line, " \t\n");/*word reception*/
        if(word==NULL || word[0]==';')/*it is an empty line, or a comment line*/
        {
            continue;
        }
        if(word[strlen(word)-1]==':')/*last char is ':' means it's a label*/
        {
            word[strlen(word)-1]='\0';
            if(find_symbol(word, *symbols_table, current_symbol_count))/*label name already exists*/
            {
                printf("error: file %s line %d label name already exists\n", file_name, line_num);
                error=TRUE;
            }
            
            else if(!label_syntax(word, line_num) || ((is_data(word))!=INT_MIN) || (is_instruction(word, instruction_table)!=INT_MIN) || (is_register(word)!=-1))/*the label is either syntactically incorrect or is the name of a command (code or data), or a register*/
            {
		printf("error: file %s line %d label name is not valid\n", file_name, line_num);
                error=TRUE;
            }
            else/*a new valid label*/ 
            {
                if(current_symbol_count>=symbol_capacity)/*if the symbol table is full, perform a reallocation*/
                {
                    symbol_capacity*=2;
                    temp_realloc=realloc(*symbols_table,(size_t)(symbol_capacity)*sizeof(symbols));
                    if(!temp_realloc)/*reallocation failed*/
                    {
                        free(*symbols_table);
                        *symbols_table = NULL;
                        printf("realloc failed\n");
                        fclose(file_i);
                        fclose(file);
                        fclose(file_d);
                        remove("file_data");
                        remove("file_instruction");
                        exit(1);
                    }
                    *symbols_table=temp_realloc;
                }
                in_label=TRUE;
		(*symbols_table)[current_symbol_count].symbol_name[0]='\0';
		(*symbols_table)[current_symbol_count].address=0;
		(*symbols_table)[current_symbol_count].type[0]='\0';
                strcpy((*symbols_table)[current_symbol_count].symbol_name, word);/*save symbol name*/
            }
            word=strtok(NULL, " \t\n");/*move to next word*/
            if(word==NULL)
            {
                printf("error: file %s line %d label without command or data following it\n",file_name, line_num);
                error=TRUE;
		continue;
            }
        } 
	rest_of_line=strtok(NULL, "\n");    

        if((ins_index=is_instruction(word, instruction_table))!=INT_MIN)/*instruction*/
        {
            char * binary_word;
            if(in_label)
            {
                (*symbols_table)[current_symbol_count].address=(ic+1);/*update the ic address in symbol table*/
                strcpy((*symbols_table)[current_symbol_count].type, "instruction");/*update type to  code*/
                current_symbol_count++;
            }
	

            if(instruction_table[ins_index].operand_num == 0)
            {
                binary_word = zero_operand_funct(rest_of_line, line_num, instruction_table[ins_index], use_label_table);            
            }
            else
            {
                if(instruction_table[ins_index].operand_num == 1)
                {
                    binary_word = one_operand_funct(rest_of_line, line_num, instruction_table[ins_index], use_label_table);            
                }
                else/*instruction_table[ins_index].operand_num == 2*/
                {
                    binary_word = two_operand_funct(rest_of_line, line_num, instruction_table[ins_index], use_label_table);            
                }

            }
	    if(strcmp(binary_word,"error")!=0)
	    {
		if((ic+1+dc+(int)((strlen(binary_word)-1)/WORD_SIZE))>MAX_LINES || exit_flag==TRUE)/*there is no space in memory or exit required*/
	    	{
			fclose(file_i);
			fclose(file);
			fclose(file_d);
			remove("file_data");
			remove("file_instruction");
			exit(1);
	    	}
	    	ic=ic+1+(int)((strlen(binary_word)-1)/WORD_SIZE);/*instructins counter promotion*/
	    	fprintf(file_i, "%s", binary_word);/*inserting the binary encoding of the binary word into the instruction file*/ 
		free(binary_word);
	    }
	    else
	    {
		error=TRUE;
	    }

	}

        else
        {
            if((data_index=is_data(word))!=INT_MIN)/*data*/
            {
                if(in_label && (data_index == ENTRY_INDEX || data_index== EXTERN_INDEX))/*Setting a label for an entry or external prompt is irrelevant*/
                {
                    (*symbols_table)[current_symbol_count].symbol_name[0]='\0';
                }
                if(in_label && data_index != ENTRY_INDEX && data_index != EXTERN_INDEX)/*if this is a label definition that is not an entry or extern*/
                {
                    (*symbols_table)[current_symbol_count].address=(dc+1);/*update the dc address in symbol table */
                    strcpy((*symbols_table)[current_symbol_count].type, "data");/*update type to  data*/
                    current_symbol_count++;
                }
                /*rest_of_line=word+strlen(word)+1; points to the place where the data content begins*/
                if(data_index != EXTERN_INDEX)/*it's not '.extern' prompt*/
                {
                    data_table[data_index].func(line_num, rest_of_line, file_d, entry_table, extern_table, instruction_table);/*running the function that handles the command*/
                    if(exit_flag==TRUE)/*if exit required*/
                    {
                        fclose(file_i);
                        fclose(file);
                        fclose(file_d);
                        remove("file_data");
                        remove("file_instruction");
                        exit(1);
                    }
                }
                else/*it's '.extern' prompt*/
                {
                    data_table[data_index].func(line_num, rest_of_line, file_d, entry_table, extern_table, instruction_table);/*running the function that handles the command*/
                    if(exit_flag==TRUE)/*if exit required*/
                    {
                        fclose(file_i);
                        fclose(file);
                        fclose(file_d);
                        remove("file_data");
                        remove("file_instruction");
                        exit(1);
                    }
                }
            }
            else/*error*/
            {
                printf("error: file %s line %d incorrect command\n",file_name, line_num);
                error=TRUE;
            }

        }
        in_label=FALSE;
    }
   

    for(k=0; k<current_symbol_count; k++)/*update dc in symbol table - add ic*/ 
    {
        if(strcmp((*symbols_table)[k].type, "data")==0)
        {
            (*symbols_table)[k].address=(*symbols_table)[k].address+ic;
        }
    }

    combine_files(file_i, file_d);/*combining the instruction file and the data file into one file*/
    fclose(file_d);
    remove("file_data");
    temp_name=replace_extention(file_name,".fp");
    rename("file_instruction", temp_name);
    free(temp_name);
    fclose(file_i);
    fclose(file);
    return TRUE;   
}



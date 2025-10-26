#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

int second_pass(char * fp_file_name, symbols * symbols_table, char entry_table[][MAX_LABEL], char extern_table[][MAX_LABEL], use_label use_label_table[])
{
    
    /*go over the lines of the ins file with file_fp, update the label addresses and the entries and externals files*/
    int ic_current=100;/*counter for line number of this file*/
    int i;
    int sym_index, use_index, line_num, ent_index;
    char label_name[MAX_LABEL];
    char line[LINE_LENGTH+2]={0};
    char * temp_num;/*saves the file name after changing the extension*/
    char am_file_name [MAX_NAME+EXTENTION] ;
    char ent_file_name [MAX_NAME+EXTENTION];
    char ext_file_name [MAX_NAME+EXTENTION] ;
    FILE * ent_file;
    FILE * ext_file;
    FILE * file_fp;
    FILE * file_write;
    temp_num=replace_extention(fp_file_name,".am");
    strcpy( am_file_name, temp_num);
    temp_num=replace_extention(fp_file_name,".ent");
    strcpy( ent_file_name, temp_num);
    temp_num=replace_extention(fp_file_name,".ext");
    strcpy(ext_file_name, temp_num);
    ent_file = fopen(ent_file_name, "w");
    ext_file = fopen(ext_file_name, "w");
    file_fp = fopen(fp_file_name, "r+");
    file_write=fopen("final", "w");
    free(temp_num);

    if(ent_file==NULL || ext_file==NULL || file_fp==NULL || file_write==NULL)
    {
        printf("error PC was unable to open one of the files \n");
        if(ent_file){fclose(ent_file);}
        if(ext_file){fclose(ext_file);}
        if(file_fp){fclose(file_fp);}
        if(file_write){fclose(file_write);}
        remove(fp_file_name);
        exit(1);
    }

    
    for(i=0; i<entries_count; i++)/*going over the labels defined as entries and inserting them into the entries file*/
    {
        sym_index=in_symbol_table(entry_table[i], symbols_table);
        if(sym_index!=INT_MIN)/*the label exists in symbols_table*/
        {
            char * binary_address=to_binary(symbols_table[sym_index].address, BITS_OF_ADDRESS);/*convert address to binary*/
	    binary_address=to_special_binary(binary_address, BITS_OF_ADDRESS);                 
	    fprintf(ent_file, "%s", symbols_table[sym_index].symbol_name);/*insertion into entries file*/
	    fprintf(ent_file, "	   %s", binary_address);/*insertion into entries file*/
            free(binary_address);
        }
    }

    for(ic_current=100;( ic_current<=ic && (fgets(line, LINE_LENGTH+2, file_fp)!=NULL)); ic_current++)/*stepping through all lines of instruction coding*/
    {
        if(line[0]=='?')/*label in instruction file*/
        {
            use_index=in_use_table(ic_current, use_label_table);/*finding the label details*/
            line_num=use_label_table[use_index].line_num;
            strcpy(label_name, use_label_table[use_index].name);
            sym_index=in_symbol_table(label_name, symbols_table);
            if((ent_index=in_entry_table(label_name, entry_table))!=INT_MIN)/*the label exists in entry_table*/
            {
                 if(sym_index==INT_MIN)/*the label doesn't exist in symbol_tabel*/
                 {
                     printf("error: file %s line %d the label defined as entry was not defined in this file\n", am_file_name ,line_num);
                     error=TRUE;
                 }
                 else
                 {
		     update_address(file_fp, symbols_table[sym_index].address);/*updating the address in the instructions file*/
                 }
            }

	    if(in_extern_table(label_name, extern_table))/*the label exists in extern_table*/
	    {
		if(in_symbol_table(label_name, symbols_table)!=INT_MIN || in_entry_table(label_name, entry_table)!=INT_MIN)/*the label exists in symbol or entry table*/
		{
		    printf("error: file %s line %d the label is defined as external but is internal\n",  am_file_name ,line_num);
		    error=TRUE; 
		}
		else
		{
		    long int temp_pos;
		    char * binary_address=to_binary(use_label_table[use_index].ic, BITS_OF_ADDRESS);/*convert address to binary*/ 
		    binary_address=to_special_binary(binary_address, BITS_OF_ADDRESS);               
		    fprintf(ext_file, "%s", label_name);/*insertion into externals file*/
		    fprintf(ext_file, "	   %s", binary_address);/*insertion into externals file*/
		    free(binary_address);
		    update_address(file_fp, 00000000);/*updating the address in the instructions file*/
		    temp_pos=ftell(file_fp);
		    fseek(file_fp, temp_pos-3, SEEK_SET);/*put the pointer before the address*/   
		    fprintf(file_fp, "%s", "01");/*update label address in the instruction file*/
		    fseek(file_fp, temp_pos, SEEK_SET);
		}
	    }
	    else 
	    {
		if(sym_index!=INT_MIN)/*it is a regular label*/
	    	{
	       		update_address(file_fp, symbols_table[sym_index].address);/*updating the address in the instructions file*/
	    	}
	    	else/*the word isn't a label name*/
	    	{
			printf("error: file %s line %d label name does not exist\n",  am_file_name ,line_num);
			error=TRUE;
	    	}
            }
	}
        
    }
    /*if any of the external entry files are empty, or errors are found, we will delete them:*/
    if(ftell(ent_file)==0 || error)/*the file is still at the beginning of the file, meaning the file is empty*/
    {
        fclose(ent_file);
        remove(ent_file_name);
    }
    else
    {
        fclose(ent_file);
    }
    if(ftell(ext_file)==0)/*the file is still at the beginning of the file, meaning the file is empty*/
    {
        fclose(ext_file);
        remove(ext_file_name);
    }
    else
    {
        fclose(ext_file);
    }
    if(error==TRUE)/*an error was detected on the first or second pass*/
    {
        fclose(file_fp);
        fclose(file_write);
        remove("final");
        remove(fp_file_name);
        return FALSE;
    }

    rewind(file_fp);
    file_to_special_binary(file_fp, file_write);/*convert to special basis*/
    temp_num=replace_extention(fp_file_name, ".ob");
    rename("final", temp_num);
    free(temp_num);
    fclose(file_fp);
    fclose(file_write);
    remove(fp_file_name);  
    return TRUE;
}

/*the function checks if the received word is the name of a label*/
int in_symbol_table(char * label, symbols * symbols_table)
{
    int i;
    for(i=0; i<current_symbol_count; i++)
    {
        if(strcmp(label, symbols_table[i].symbol_name)==0)/*the label exists in symbol_table*/
        return i;
    }
    return INT_MIN;
}

/*the function checks if the received word is the name of an 'entry' label*/
int in_entry_table(char * label, char entry_table[][MAX_LABEL])
{
    int i;
    for(i=0; i<entries_count; i++)
    {
        if(strcmp(label, entry_table[i])==0)/*the label exists in entry_table*/
        return i;
    }
    return INT_MIN;
}

/*the function checks if the received word is the name of an 'extern' label*/
int in_extern_table(char * label, char extern_table[][MAX_LABEL])
{
    int i;
    for(i=0; i<externals_count; i++)
    {
        if(strcmp(label, extern_table[i])==0)/*the label exists in extern_table*/
        return TRUE;
    }
    return FALSE;
}

/*the function checks whether a received word address is a label address*/
int in_use_table(int ic_current, use_label use_label_table[])
{
    int i;
    for(i=0; i<use_label_count; i++)
    {
        if(use_label_table[i].ic==ic_current)
        {
            return i;
        }
    }
    return INT_MIN;
}

/*the function updates the label address*/
void update_address(FILE * file_fp, int new_address)
{
    long int temp_pos;
    char * binary_address;
    temp_pos=ftell(file_fp);
    fseek(file_fp, temp_pos-BITS_OF_ADDRESS-3, SEEK_SET);/*put the pointer before the address*/
    binary_address=to_binary(new_address, BITS_OF_ADDRESS);/*convert address to binary*/   
    fprintf(file_fp, "%s", binary_address);/*update label address in the instruction file*/
    free(binary_address);
    fseek(file_fp, temp_pos, SEEK_SET);/*returning the pointer to the end of the line*/
}

/*the function converts each line in a file to a special binary base*/
void file_to_special_binary(FILE * file_fp, FILE * file_write)
{
    int line_num=100;
    char line[LINE_LENGTH+2]={0};
    char * bin_ic=to_binary(ic-99,LINE_LENGTH);
    char * bin_dc=to_binary(dc,LINE_LENGTH);
    bin_ic=to_special_binary(bin_ic,LINE_LENGTH);
    bin_dc=to_special_binary(bin_dc,LINE_LENGTH);
    bin_ic[strlen(bin_ic)-1]='\0';
    bin_ic=delete_leadings(bin_ic);
    bin_dc=delete_leadings(bin_dc);
    fprintf(file_write, "  %s  %s", bin_ic, bin_dc);/*inserting the instruction and data counters to the file's title*/
    free(bin_ic);
    free(bin_dc);
    while((fgets(line, LINE_LENGTH+2, file_fp)!=NULL))/*binary line reception*/
    {
        char * con_line=to_special_binary(line,LINE_LENGTH);
        char * bin_line=to_binary(line_num, BITS_OF_ADDRESS);
        bin_line=to_special_binary(bin_line, BITS_OF_ADDRESS);
        bin_line[strlen(bin_line)-1]='\0';
        fprintf(file_write, "%s    %s", bin_line, con_line);/*inserting the row into place after conversion*/
        free(bin_line);
        free(con_line);
        line_num++;
    }  
}

/*the function accepts a binary string of a certain length and returns it in a special binary base*/
char * to_special_binary(char * str, int length)
{
    char convert[ROWS][COLS][BITS_TO_CONVERT+1] = {
        {"00", "a"},
        {"01", "b"},
        {"10", "c"},
        {"11", "d"}
    };
   
    char * special_binary=malloc(length/2+2);/*every 2 binary digits are converted to one special binary character*/
    int i, row;
    for(i=0; i<length; i+=2)/*convert every 2 bits to one character*/
    {
        char temp [BITS_TO_CONVERT+1];
        temp[0]=str[i];
        temp[1]=str[i+1];
        temp[2]='\0';
        for(row=0; row<ROWS; row++)
        {
            if(strcmp(convert[row][0], temp)==0)/*finding the matching conversion*/
            {
                special_binary[i/2]=convert[row][1][0];/*adding a character to a special binary string*/
                break;
            }
        }
    }
    special_binary[i/2]='\n';
    special_binary[i/2+1]='\0';
    return special_binary;      
}

/*the function accepts a string of IC or DC and removes unnecessary leading 'a'*/
char * delete_leadings(char * str)
{
    int i, count=0;
    int char_flag=FALSE;/*indicates whether there was a character other than 'a'*/
    char * shorter = malloc(WORD_SIZE);
    if(!shorter)/*allocation failed*/
    {

    }
    for(i=0; str[i+1]!='\0'; i++)/*all leading 'a' is not copied*/
    {
        if(str[i]!='a' || char_flag)/*not 'a' or a non-leading 'a'*/
        {
            shorter[count]=str[i];
            count++;
            char_flag=TRUE;
        }
    }
    shorter[count]=str[i];/*copy the last letter*/
    count++;
    shorter[count]='\0';
    return shorter;
}



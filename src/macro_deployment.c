#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

/*replaces macro calls with their content and writes the processed output to a new file*/
int macro_deployment(int argc, char* file_name, char * am_file_name, data * data_table, instruction * instruction_table)
{
    FILE *file;
    FILE * temp;
    FILE *file_am;

    macro * temp_realloc=NULL; /* temporary pointer for reallocation*/
    char line[MAX_LINE]={0};
    char line_copy[MAX_LINE];
    char * word;/*current word*/
    
    int in_macro=FALSE;/*flag true when in macro body and false otherwise*/
    int current_array_size=0;
    int array_size=0;
    long int start_line;/*temporary*/
    int index=INT_MIN;
    char * position;
    int line_num=1;
    
    /*creating a structure that will hold macro names and the begining and end points of each macro in the macro file*/
    macro *macro_array; 
    file = fopen(file_name, "r");/*open file*/
    temp = fopen(file_name, "r");/* temporary pointer for copying macros*/
    file_am = fopen(am_file_name, "w");/*open an output file for writing*/

    if(file==NULL || file_am==NULL || temp==NULL)/*checks if all files can open*/
    {
        printf("error PC was unable to open one of the files \n");
        if(file){fclose(file);}
        if(temp){fclose(temp);}
        if(file_am)
	{
		fclose(file_am);
		remove(am_file_name);
	}
        exit(1);
    }
    macro_array = malloc(sizeof(macro));
    if (macro_array == NULL) 
    {
        printf("malloc failed");
        fclose(file);
        fclose(temp);
        fclose(file_am);
        remove(am_file_name);
        exit(1);
    }
    array_size++;
    if(argc < MIN_ARGC) /*file name not entered*/
    {
        printf("error no file name\n");
        return error=TRUE;
    }    
   

    start_line=ftell(file);/*saves the begining of line*/
    while(fgets(line, MAX_LINE, file)!=NULL)/*as long as we haven't reached the end of the file*/
    {        
        if(strlen(line)==0)/*empty line*/
        {
            next_line(file);
            continue;
        }
        if(line[strlen(line)-1]!='\n' && line[strlen(line)-1]!='\0')/*the line is longer than 80 characters*/
        {
            printf("error: file %s line %d  more than 80 characters\n" ,file_name, line_num);
            next_line(file);
            continue;
        }
        strcpy(line_copy, line);
        word=strtok(line_copy, " \t\n");/*word reception*/
        if(word==NULL)
        {
            continue;
        }
        if(strcmp(word,"mcroend")==0)/*if got to end of macro*/
        {
            macro_array[current_array_size].end=start_line;/*save macro end in macro array*/
            current_array_size++;
            in_macro=FALSE;
            position=line+(word - line_copy)+strlen(word);
            if( *position!='\n') /*check if line ends after word*/
            {
                printf("error: file %s line %d excces of chars in line\n",file_name, line_num);
                error=TRUE;
            }

        }
        else
        { 
            if(strcmp(word, "mcro")==0)/*if macro begins here*/
            {
                in_macro=TRUE;
                word=strtok(NULL, " \t\n");/*word reception*/
                if(word==NULL)
                {
                    printf("error: file %s line %d defining a macro without a name\n",file_name, line_num);
                    error=TRUE;
                    continue;
                }
                if(is_data(word)!=INT_MIN || (is_instruction(word, instruction_table)!=INT_MIN) || is_register(word)!=-1)/*checking if the macro is a command or data name or register name*/
                {
                    printf("error: file %s line %d the macro name is a reserved name\n",file_name, line_num);
                    error=TRUE;
                    continue;
                }
                if(current_array_size==array_size)/*if the macro table is full, perform a reallocation*/
                {
                    array_size*=2;
                    temp_realloc=realloc(macro_array, (size_t)array_size*sizeof(macro));
                    if(!temp_realloc)/*reallocation failed*/
                    {
                        free(macro_array);
                        printf("realloc failed\n");
                        fclose(file);
                        fclose(temp);
                        fclose(file_am);
                        remove(am_file_name);
                        exit(1);
                    }
                    macro_array=temp_realloc;
                }

                strcpy(macro_array[current_array_size].name, word);/*save macro name in macro array*/
                macro_array[current_array_size].start= ftell(file);
                /*macro_array[current_array_size].start=start_line+(word-line)+strlen(word); save macro start in macro array*/
                /*printf("start line is %d , word-line is %d\n", start_line, word-line);*/
                position=line+(word - line_copy)+strlen(word);
                if( *position!='\n') /*check if line ends after word*/
                {
                    printf("error: file %s line %d excces of chars in line\n",file_name, line_num);
                    error=TRUE;
                }
            }
            else
            {
                if(!in_macro)
                {
                    index=find_macro(macro_array, word, current_array_size);/*search for the macro name in the macro table*/
                    if(index!=INT_MIN)/*macro call*/
                    {
                        fseek(temp, macro_array[index].start, SEEK_SET);/*sets temporary pointer to macro begining*/
                        while(ftell(temp) < macro_array[index].end)/*copy macro to the output file*/
                        { 
                            if(fgets(line, MAX_LINE, temp) != NULL)
                            {
                                fputs (line, file_am);/*copy rest of line*/
                            }
                        }

                    }
                    else/* regular text line*/
                    {
                        /*copy the line to the output file:*/
                        fputs (line, file_am);/*copy rest of line*/
                    }    
                }              
            }
        } 
        line_num++;
        start_line=ftell(file);/*saves the begining of line*/
   }

   
   free(macro_array);
   fclose(file);
   fclose(temp);
   fclose(file_am);
   
    if(error==TRUE)
    {
        remove(am_file_name);/*delete am file*/
    }
    return ERROR;
}

/*The find_macro function searches for a macro with the given 
word as its name in the macro_array, and returns its index if
found, or INT_MIN if not*/
int find_macro(macro* macro_array, char * word, int current_array_size)
{
    int i=0;
    for(; i<current_array_size; i++)
    {
        if(strcmp(word,macro_array[i].name)==0)
        {
            return i;
        }
    }
    return INT_MIN;
}


/* function appends a file extension type to the end of file_name 
and returns the resulting new string*/
char * replace_extention(char * file_name, char* type)
{
    /* calculate total length (+1 for null terminator)*/
    size_t file_name_length = strlen(file_name);
    size_t type_length = strlen(type);
    size_t total_length = file_name_length + type_length + 1;
    /*allocate memory*/
    char * new_name = malloc(total_length);
    char copy [MAX_NAME+EXTENTION];
    strcpy(copy, file_name);
    if (new_name == NULL)
    {
        printf("malloc failed");
    }

    /*copy and concatenate*/
    strcpy(new_name, strtok(copy,"."));
    strcat(new_name, type);
    return new_name;
}

/*the function moves the cursor to the beginning of the next line*/
void next_line(FILE * file)
{
    int ch;
    while ((ch = fgetc(file)) != '\n' && ch != EOF);
}

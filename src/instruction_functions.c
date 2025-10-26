#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

/*if first word in line is stop it sends the rest of the line to the stop function*/
char* zero_operand_funct(char * line, int line_num ,instruction inst, use_label use_label_table[])
{
    char opcode[OPCODE_SIZE]; /*initialize binary_word to NULL*/
    char operands[8]="000000\n"; /*initialize operands to 000000/n*/
    char * binary_word = malloc(MAX_LINE+1); /*initialize binary_word to NULL*/
    if (!binary_word)
    {
        perror("malloc failed");
        exit_flag=TRUE;
        return "error";
    }
    /* function to check if a line ends here */
    if(!line_ends_here(line))
    {
        printf("error: file %s line %d has too many operands\n",cur_file,  line_num);
        return "error"; /* indicates that the line is not empty and contains non-space characters */
    }
    /* nothing after 'stop' except spaces/tabs */
    strcpy(opcode, inst.opcode_binary); /* binary code for stop instruction */
    /*connect opcode and operands*/
    strcpy(binary_word, opcode); /*copy opcode to binary_word*/
    strcat(binary_word, operands);/*connect opcode and operands*/     
    return binary_word; /*return the binary word*/
}



char* one_operand_funct(char * line, int line_num ,instruction inst, use_label use_label_table[])
{
    int operand_type = NO_OPERAND;
    
    int num_lines = 0;
    int num=0;/*used for turning char to int*/

    char opcode[OPCODE_SIZE]={0}; /*initialize opcode to the binary code of the instruction*/
    char first_operand[OPERAND_SIZE] ="00";
    char second_operand[OPERAND_SIZE];
    char era[OPERAND_SIZE] = "00"; /*initialize era to 00*/

    char * binary_word ;
    char extra_line [(WORD_SIZE * 2)+1]={0} ;
    char * operand; /*allocate memory for operand*/
    char  * rest_of_line; /*allocate memory for rest of_line*/
    char cpy_line[MAX_LINE]={0};
    
    char  * mat_name ; /*allocate memory for matrix name*/
    char  reg [MAX_LINE]="[";
    char *  registers;
    
    strcpy(opcode,inst.opcode_binary);

    if(line_ends_here(line)) /*if the line ends here and commas are not correct*/
    {
        printf("error: file %s line %d has too few operands\n",cur_file,  line_num);
        return "error";
    }

    /*get next word*/
    strcpy(cpy_line,line);     
    strcat(cpy_line,"\n");/*to avoid overflow*/
    operand=strtok(cpy_line, " \t\n");/*get the operand from the line*/
    rest_of_line=strtok(NULL, "\n");
	
    
   if(is_mat(operand))
    {
        strcpy(cpy_line,line);     
        strcat(cpy_line,"\n");/*to avoid overflow*/
        operand=strtok(cpy_line, "\n");/*get the operand from the line*/
        rest_of_line="\n\0"; /*get the rest of the line*/
    }


    /*check if legal type*/
    if(operand[0] == '#'&& strcmp("prn", inst.name) == 0)/*direct && prn*/
    {
        operand_type = IMMEDIATE_ADD; /*if the operand is an immediate addressing mode, set the operand type to immediate addressing mode*/
        strcpy(second_operand,"00");  /*if the operand is an immediate addressing mode, set the second operand type to immediate addressing mode*/
        num = immediate(operand,line_num ); /*check if the operand is a number and return it*/
        if(num == INT_MIN)
        {
            return "error";
        }
        strcpy(extra_line, to_binary(num, NUMBER_SIZE)); /*convert the operand to binary*/
        if(exit_flag)
        {
           return "error"; 
        }
        strcat(extra_line, era); /*connect "00" to the end of the extra line*/
        strcat(extra_line, "\n"); /*connect \n to the end of the extra line*/
    }
    else
    {
        if(is_register(operand)!=-1)
        {
            strcpy(second_operand,"11");  /*if the operand is a register, set the second operand type to register addressing mode*/
            operand_type = REGISTER_ADD; /*if the operand is a register, set the operand type to register addressing mode*/
            strcpy(extra_line, reg_to_word(0, is_register(operand))); /*convert the operand to binary*/
            if(exit_flag)
            {
                return "error";
            }
        }
        else/*is a label*/
        {
            if(is_mat(operand))/*is a matrix label*/
            {
                FILE * temp = fopen("temp_file","w");/*opening a temporary file for matrix treatment use*/
                if(!temp)
                {
                    exit_flag=TRUE;
		
                    return "error";
                }

                strcpy(second_operand,"10"); /*if the operand is a matrix label, set the second operand type to matrix addressing mode*/
                operand_type = MATRIX_ADD; /*if the operand is a matrix label, set the operand type to matrix addressing mode*/
               
                /*check matrix name*/
                mat_name = strtok(operand,"[");
		strcat(reg, strtok(NULL, "\0"));
             	strcpy(extra_line, "?00000010\n"); /*allocate memory for extra_line*/
                
                registers=matrix_treatment(line_num, reg, temp, "instructions");
                if(exit_flag)
                {
                    fclose(temp);
                    remove("temp_file");  
                    return "error";
                }
		strcat(extra_line, registers);
                if(strlen(registers)==0)
                {
		    fclose(temp);
    		    remove("temp_file");
                    return "error";
                }
                fclose(temp);
                remove("temp_file");          
                strcpy(use_label_table[use_label_count].name, mat_name );
                use_label_table[use_label_count].ic=ic+2;/*ic of the label*/
                use_label_table[use_label_count].line_num=line_num;
                use_label_count++;

            }
            else
            {
		if(operand[0] == '#')/*invalid addressing method for this instruction*/
                {
                    printf("error: line %d invalid addressing method for this instruction\n", line_num);
                    return "error"; /*return false if the addressing method is invalid*/
                }
		if(operand[0]==',')
		{
		    printf("error: line %d too many commas\n", line_num);
                    return "error"; /*return false if the addressing method is invalid*/
		}
                if(label_syntax(operand, line_num)==FALSE)/*illegal label name or is a saved word*/
                {
                    return "error"; /*return false if the label name is illegal*/
                } 
                  
                strcpy(second_operand,"01");  /*if the operand is a label, set the second operand type to direct addressing mode*/
                operand_type = DIRECT_ADD; /*if the operand is a label, set the operand type to direct addressing mode*/
                strcpy(extra_line, "?000000010\n"); /*allocate memory for extra_line*/
                strcpy(use_label_table[use_label_count].name, operand);
                use_label_table[use_label_count].ic=ic+2;/*ic of the label*/
                use_label_table[use_label_count].line_num=line_num;
                use_label_count++;
            }
                
        }

    }
   if(!line_ends_here(rest_of_line ))/*function to check if a line ends here */
    {
        printf("error: file %s line %d has too many operands\n",cur_file,  line_num);
        return "error";
    }

    num_lines = num_of_lines(operand_type, NO_OPERAND, 1); /*calculate the number of lines needed for the instruction*/
    binary_word = malloc((size_t)((WORD_SIZE * num_lines)+1)); /*allocate memory for binary_word*/
    if (!binary_word)
    {
        perror("malloc failed");
        free(binary_word);
        exit_flag=TRUE;
        return "error";
    }

    strcpy(binary_word, opcode); /*copy opcode to binary_word*/
    strcat(binary_word, first_operand);/*connect empty*/     
    strcat(binary_word, second_operand);/*connect operand*/ 
    strcat(binary_word, era);/*connect era*/ 
    strcat(binary_word, "\n"); /*connect \n to the end of the binary word*/
    strcat(binary_word, extra_line);/*connect extra line/lines*/
    
    return binary_word; /*return the binary word*/
}


char * two_operand_funct(char * line, int line_num ,instruction inst, use_label use_label_table[])
{
    int first_operand_type;
    int second_operand_type;
    
    int num_lines = 0;
    long num=0;/*used for turning char to int*/

    char opcode[OPCODE_SIZE]={0};/*initialize opcode to the binary code of the instruction*/
    char first_operand[OPERAND_SIZE];
    char second_operand[OPERAND_SIZE];
    char era[OPERAND_SIZE]= "00"; /*initialize era to 00*/

    char * binary_word ;
    char extra_line [(WORD_SIZE * 2)+1]={0};
    char extra_line_2 [(WORD_SIZE * 2)+1]={0} ;
    char * operand_temp; /*allocate memory for operand*/
    char * operand_1; /*allocate memory for operand*/
    char * operand_2; /*allocate memory for operand*/
    char * rest_of_line; /*allocate memory for rest of_line*/
    char cpy_line_1[MAX_LINE]={0};
    char cpy_line_2[MAX_LINE]={0};

    
    char  * mat_name ; /*allocate memory for matrix name*/
    char  reg [MAX_LINE]="[";
    char *  registers;

    FILE * temp = fopen("temp_file","w");/*opening a temporary file for matrix treatment use*/
    if(!temp)
    {
        exit_flag=TRUE;
    }
    
    strcpy(opcode,inst.opcode_binary);

    if(line_ends_here(line)) /*if the line ends here and commas are not correct*/
    {
        printf("error: file %s line %d has too few operands\n",cur_file,  line_num);
	fclose(temp);
        remove("temp_file");
        return "error";
    }

    /*get next word*/
    strcpy(cpy_line_1,line);     
    strcat(cpy_line_1,"\n");/*to avoid overflow*/
    operand_temp=strtok(cpy_line_1, ",");/*get the operand from the line*/
    rest_of_line=strtok(NULL, "\n");
    operand_1=strtok(operand_temp, ", \t\n");/*get the operand from the line*/

    if(is_mat(operand_1))
    {
        strcpy(cpy_line_1,line);     
        strcat(cpy_line_1,"\n");/*to avoid overflow*/
        operand_1=strtok(cpy_line_1, ",");/*get the operand from the line*/
        rest_of_line=strtok(NULL, "\n\0");    
    }


    if(line_ends_here(rest_of_line)) /*if the line ends here and commas are not correct*/
    {
        printf("error: file %s line %d has too few operands\n",cur_file,  line_num);
        fclose(temp);
        remove("temp_file");
        return "error";
    }

    /*get next word*/
    strcpy(cpy_line_2,rest_of_line);     
    strcat(cpy_line_2,"\n");/*to avoid overflow*/
    operand_2=strtok(cpy_line_2, " \t\n");/*get the operand from the line*/
    rest_of_line=strtok(NULL, "\n");

    if(is_mat(operand_2))
    {
        strcpy(cpy_line_2,rest_of_line);     
        strcat(cpy_line_2,"\n");/*to avoid overflow*/
        operand_2=strtok(cpy_line_2, ",");/*get the operand from the line*/
        rest_of_line="\n\0";    
    }

    if(operand_1[0] == '#' && (strcmp("lea", inst.name) != 0))/*direct && not lea*/
    {
        first_operand_type = IMMEDIATE_ADD; /*if the operand is an immediate addressing mode, set the operand type to immediate addressing mode*/
        strcpy(first_operand,"00"); /*if the operand is an immediate addressing mode, set the first operand type to immediate addressing mode*/
        num = immediate(operand_1, line_num); /*check if the operand is a number and return it*/
        if(num == INT_MIN)
        {
            fclose(temp);
    	    remove("temp_file");
            return "error";
        }
        strcpy(extra_line, to_binary(num, NUMBER_SIZE)); /*convert the operand to binary*/
        if(exit_flag)
        {
            fclose(temp);
            remove("temp_file");  
            return "error"; 
        }
        strcat(extra_line, era); /*connect "00" to the end of the extra line*/
        strcat(extra_line, "\n"); /*connect \n to the end of the extra line*/
    }
    else
    {
        if(is_register(operand_1)!=-1 && strcmp("lea", inst.name) != 0)
        {
            strcpy(first_operand , "11"); /*if the operand is a register, set the second operand type to register addressing mode*/
            first_operand_type= REGISTER_ADD; /*if the operand is a register, set the second operand type to register addressing mode*/
            strcpy(extra_line, reg_to_word(is_register(operand_1), 0)); /*create extra line */
            if(exit_flag)
            {
                fclose(temp);
                remove("temp_file");  
                return "error"; 
            }
        }
        else
        {
            if(is_mat(operand_1))/*is a matrix label*/
            {
                strcpy(first_operand,"10");  /*if the operand is a matrix label, set the second operand type to matrix addressing mode*/
                first_operand_type = MATRIX_ADD; /*if the operand is a matrix label, set the operand type to matrix addressing mode*/
                mat_name = strtok(operand_1,"[");
                strcat(reg, strtok(NULL, "\0"));
                strcpy(extra_line, "?000000010\n"); /*allocate memory for extra_line*/
                
                registers=matrix_treatment(line_num, reg, temp, "instructions");  
                if(exit_flag)
                {
                    fclose(temp);
                    remove("temp_file");  
                    return "error";
                } 
                
                if(strlen(registers)==0)                
                {
                    fclose(temp);
                    remove("temp_file");
                    return "error";
                }
                 
                strcat(extra_line, registers);
                strcpy(use_label_table[use_label_count].name, mat_name);
                use_label_table[use_label_count].ic=ic+2;/*ic of the label*/
                use_label_table[use_label_count].line_num=line_num;
                use_label_count++; 
            }
            else
            {
		if(operand_1[0] == '#'|| is_register(operand_1)!=-1)/*invalid addressing method for this instruction*/
                {
                    printf("error:file %s line %d invalid addressing method for this instruction\n",cur_file, line_num);
		    fclose(temp);
    		    remove("temp_file");
                    return "error"; /*return false if the addressing method is invalid*/
                }
		if(operand_1[0]==',')
		{
		    printf("error: line %d too many commas\n", line_num);
		    fclose(temp);
    		    remove("temp_file");
                    return "error"; /*return false if the addressing method is invalid*/
		}
                if(label_syntax(operand_1,line_num)==FALSE)/*illegal label name or is a saved word*/
                {
		    fclose(temp);
    		    remove("temp_file");
                    return "error"; /*return false if the label name is illegal*/
                }  
                strcpy(first_operand,"01");  /*if the operand is a label, set the second operand type to direct addressing mode*/
                first_operand_type = DIRECT_ADD; /*if the operand is a label, set the operand type to direct addressing mode*/
                strcpy(extra_line, "?000000010\n"); /*allocate memory for extra_line*/
                strcpy(use_label_table[use_label_count].name, operand_1);
                use_label_table[use_label_count].ic=ic+2;/*ic of the label*/
                use_label_table[use_label_count].line_num=line_num;
                use_label_count++;
            }

        }
    }

    if(operand_2[0] == '#'&& strcmp("cmp", inst.name) == 0)/*direct && cmp*/
    {
        second_operand_type = IMMEDIATE_ADD; /*if the operand is an immediate addressing mode, set the operand type to immediate addressing mode*/
        strcpy(second_operand,"00");  /*if the operand is an immediate addressing mode, set the second operand type to immediate addressing mode*/
        num = immediate(operand_2, line_num); /*check if the operand is a number and return it*/
        if(num == INT_MIN)
        {
	    fclose(temp);
            remove("temp_file");
            return "error";
        }
        strcpy(extra_line_2, to_binary(num, NUMBER_SIZE)); /*convert the operand to binary*/
        if(exit_flag)
        {
            fclose(temp);
            remove("temp_file");  
            return "error"; 
        }
        strcat(extra_line_2, era); /*connect "00" to the end of the extra line*/
        strcat(extra_line_2, "\n"); /*connect \n to the end of the extra line*/
    }
    else
    {
        if(is_register(operand_2)!=-1)
        {
            strcpy(second_operand,"11");  /*if the operand is a register, set the second operand type to register addressing mode*/
            second_operand_type= REGISTER_ADD; /*if the operand is a register, set the second operand type to register addressing mode*/
            if(is_register(operand_1)!=-1) /*if the first operand is also a register*/
            {
                strcpy(extra_line, reg_to_word(is_register(operand_1), is_register(operand_2) )); /*create extra line */
                if(exit_flag)
                {
                    fclose(temp);
                    remove("temp_file");  
                    return "error"; 
                }
            }
            else
            {
                strcpy(extra_line_2, reg_to_word(0, is_register(operand_2))); /*create extra line */
                if(exit_flag)
                {
                    fclose(temp);
                    remove("temp_file");  
                    return "error"; 
                }
            }
        }
        else
        {
            if(is_mat(operand_2))/*is a matrix label*/
            {
                strcpy(second_operand,"10"); /*if the operand is a matrix label, set the second operand type to matrix addressing mode*/
                second_operand_type = MATRIX_ADD; /*if the operand is a matrix label, set the operand type to matrix addressing mode*/
                
                /*check matrix name*/
                mat_name = strtok(operand_2,"[");
		        strcat(reg, strtok(NULL, "\0"));
                strcpy(extra_line_2, "?000000010\n"); /*allocate memory for extra_line*/

                registers=matrix_treatment(line_num, reg, temp, "instructions");
                if(exit_flag)
                {
                    fclose(temp);
                    remove("temp_file");  
                    return "error";
                } 
                strcat(extra_line_2, registers); 
                
                if(strlen(registers)==0)
                {
                    fclose(temp);
                    remove("temp_file");
                    return "error";
                }
		strcpy(use_label_table[use_label_count].name, mat_name);
                if(first_operand_type == MATRIX_ADD)
		{
			use_label_table[use_label_count].ic=ic+4;/*ic of the label*/
		}
                else
		{
                	use_label_table[use_label_count].ic=ic+3;/*ic of the label*/
		}
                use_label_table[use_label_count].line_num=line_num;
                use_label_count++; 
            }
            else
            {
		if(operand_2[0] == '#')/*invalid addressing method for this instruction*/
                {
                    printf("error: file %s line %d invalid addressing method for this instruction\n",cur_file, line_num);
		    fclose(temp);
    		    remove("temp_file");
                    return "error"; /*return false if the addressing method is invalid*/
                }
		if(operand_2[0]==',')
		{
		    printf("error: line %d too many commas\n", line_num);
		    fclose(temp);
    		    remove("temp_file");
                    return "error"; /*return false if the addressing method is invalid*/
		} 
                if(label_syntax(operand_2, line_num)==FALSE)/*illegal label name or is a saved word*/
                {
		    fclose(temp);
    		    remove("temp_file");
                    return "error"; /*return false if the label name is illegal*/
                } 
         
                strcpy(second_operand,"01");  /*if the operand is a label, set the second operand type to direct addressing mode*/
                second_operand_type = DIRECT_ADD; /*if the operand is a label, set the operand type to direct addressing mode*/
                strcpy(extra_line_2, "?000000010\n"); /*allocate memory for extra_line*/
                strcpy(use_label_table[use_label_count].name,operand_2);
		if(first_operand_type == MATRIX_ADD)
		{
			use_label_table[use_label_count].ic=ic+4;/*ic of the label*/
		}
                else
		{
                	use_label_table[use_label_count].ic=ic+3;/*ic of the label*/
		}
                use_label_table[use_label_count].line_num=line_num;
                use_label_count++;
            }
            

        }
    }

    fclose(temp);
    remove("temp_file");

    if(!line_ends_here(rest_of_line ))/*function to check if a line ends here */
    {
        printf("error: file %s line %d has too many operands\n",cur_file,  line_num);
        return "error";
    }
    
    num_lines = num_of_lines(first_operand_type, second_operand_type, 2); /*calculate the number of lines needed for the instruction*/
    binary_word = malloc((size_t)((WORD_SIZE * num_lines)+2)); /*allocate memory for binary_word*/
    if (!binary_word)
    {
        perror("malloc failed");
        free(binary_word);
        exit_flag=TRUE;
        return "error";
    }

    strcpy(binary_word, opcode); /*copy opcode to binary_word*/
    strcat(binary_word, first_operand);/*connect empty*/     
    strcat(binary_word, second_operand);/*connect operand*/ 
    strcat(binary_word, era);/*connect era*/ 
    strcat(binary_word, "\n"); /*connect \n to the end of the binary word*/
    strcat(binary_word, extra_line);/*connect extra line/lines*/
    strcat (binary_word, extra_line_2);/*connect second extra line/lines*/

    return binary_word; /*return the binary word*/    
}





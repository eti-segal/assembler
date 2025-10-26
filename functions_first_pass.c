#include "defines.h"
#include "structs.h"
#include "functions.h"
#include "globals.h"

/*the function receives a line and checks for punctuation errors*/
int correct_commas(char * rest_of_line, int line_num )
{
    int comma=FALSE, in_word=FALSE, new_word=FALSE;/*flags*/
    int i;
    for(i=0; i<strlen(rest_of_line); i++)
    {
        if(rest_of_line[i]=='\t'||rest_of_line[i]==' ')/*the character is space or tab*/
        {
            if(in_word)/*space after word*/
            {
                in_word=FALSE;
                new_word=TRUE;
            }
        }
        else if(rest_of_line[i]==',')/*the character is a comma*/
        {
            if(comma)/*two adjacent commas*/
            {
                printf("error: file %s line %d two adjacent commas\n", cur_file, line_num);
                error=TRUE;
                return FALSE;
            }
            if(!in_word && !comma && !new_word)/*comma before the first argument*/
            {
                printf("error: file %s line %d comma before the first argument\n", cur_file, line_num);
                error=TRUE;
                return FALSE;
            }
            if((in_word||new_word) && !comma)/*a correct comma*/
            {
                in_word=FALSE;
                comma=TRUE;
            }
            
        }
        else/*the character isn't a comma and isn't a white character*/
        {
            if(new_word && !comma)/*missing comma between arguments*/
            {
                printf("error: file %s line %d missing comma between arguments\n",cur_file, line_num);
                error=TRUE;
                return FALSE;
            }
            else
            {
                new_word=FALSE;
                in_word=TRUE;
                comma=FALSE;
            }
        }
    }
    if(comma)/*comma after the last argument*/
    {
        printf("error: file %s line %d comma after the last argument\n",cur_file, line_num);
        error=TRUE;
        return FALSE;;
    }
    return TRUE;
}

/*the function checks whether the received word is the name of a data command*/
int is_data(char * word)
{
    int i;
    char * data_names[DATA_QUANTITY]={".data", ".string", ".mat", ".entry", ".extern"};
    for(i=0; i<DATA_QUANTITY; i++)
    {
        if(strcmp(word, data_names[i])==0)/*the word is a data command*/
            return i;
    }
    return INT_MIN;/*the word isn't data command, return index which isn't index of data*/
}

/*the function checks whether the received word is the name of an instruction*/
int is_instruction(char * word, instruction * instruction_table)
{
    int i;
    for(i=0; i<INSTRUCTION_QUANTITY; i++)
    {
        if(strcmp(word, instruction_table[i].name)==0)/*the word is a data command*/
            return i;
    }
    return INT_MIN;/*the word isn't data command, return index which isn't index of data*/
}

/*the function checks if there is a valid matrix definition and handles its contents*/
char * matrix_treatment(int line_num, char * rest_of_line, FILE * file_d, char * type)
{
    int values=1, i, int_number;
    int reg_1=INT_MIN, reg_2=INT_MIN;
    char str_number[MAX_LINE];
    char register_1[REG_SIZE];
    char register_2[REG_SIZE];
    int l_bracket1=FALSE, l_bracket2=FALSE, r_bracket1=FALSE, r_bracket2=FALSE, num=FALSE; /*flags for brackets*/
    char * reg_word=malloc(WORD_SIZE+1);
    if (!reg_word)
    {
        perror("malloc failed");
        exit_flag=TRUE;
        return NULL;
    }
    reg_word[0]='\0';
    for(i=0; r_bracket2==FALSE && rest_of_line[i]!='\0'; i++)/*character by character until closing parentheses or end of string*/
    {
        if(rest_of_line[i]=='[')
        {
            if(!l_bracket1 && !l_bracket2)/*first left bracket*/
            {
                l_bracket1=TRUE;
            }
            else if(l_bracket1 && !l_bracket2)/*second left bracket*/
            {
                if(r_bracket1)/*second left bracket can come only if was before a close bracket*/
                {
                    l_bracket2=TRUE;
                }
                else
                {
                    printf("error: file %s line %d missing first closing bracket\n", cur_file, line_num);
                    error=TRUE;
                    return reg_word;
                }
            }
            else /*there were already 2 left brackets*/
            {
                printf("error: file %s line %d too many left brackets\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
                
            }
        }
        else if(rest_of_line[i]==' ' || rest_of_line[i]=='\t')/*a white character is valid only before or inside the brackets*/
        {
            
            if(r_bracket1 && !l_bracket2)/*if the first closing bracket isn't adjacent to the second opening bracket*/
            {
                printf("error: file %s line %d the first closing bracket must be adjacent to the second opening bracket\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
        }
        else if(isdigit(rest_of_line[i]) && strcmp(type, "data")==0)
        {
            if(num)/*we read a number in these brackets before*/
            {
                printf("error: file %s line %d two numbers in bracket\n", cur_file,line_num);
                error=TRUE;
                return reg_word;
            }
            else if(r_bracket1 && !l_bracket2)/*if the first closing bracket isn't adjacent to the second opening bracket*/
            {
                printf("error: file %s line %d the first closing bracket must be adjacent to the second opening bracket\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
            else/*first number in brackets*/
            {
                char temp[MAX_LINE];
                strcpy(temp, rest_of_line + i);/*save a copy of the continuation of the line starting from the number's position*/
                strcpy(str_number,strtok(temp, " \t]"));/*get the number*/ 
                if((int_number=is_valid_number(str_number, line_num, 1, INT_MAX))!=INT_MIN)/*the number is valid*/
                {
                    values=values*int_number;/*the number of values the matrix contains*/
                    num=TRUE; /*we read a number*/
                }
                else/*number isn't valid*/
                {
                    return reg_word;
                }
            }
            
        }
        else if(rest_of_line[i]=='r' && strcmp(type, "instructions")==0)
        {
            if(!l_bracket1)/*missing first opening bracket*/
            {
                printf("error: file %s line %d missing first opening bracket\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
            else if(r_bracket1 && !l_bracket2)/*if the first closing bracket isn't adjacent to the second opening bracket*/
            {
                printf("error: file %s line %d the first closing bracket must be adjacent to the second opening bracket\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
            else if(reg_1==INT_MIN)/*we are with the first register*/
            {
                register_1[0]=rest_of_line[i];
                if(rest_of_line[i+1]!='\0')
                {
                    register_1[1]=rest_of_line[i+1];/*getting the entire register name*/
                    register_1[2]='\0';
                    i++;/*because we read another character*/
                    if(is_register(register_1)==-1)/*not a register name*/
                    {
                        printf("error: file %s line %d the matrix does not contain a valid register name\n", cur_file, line_num);
                        error=TRUE;
                        return reg_word;
                    }
                    reg_1=TRUE;
                }
            }
            else if(reg_1!=INT_MIN && !r_bracket1)/*extra character in first brackets*/
            {
                printf("error: file %s line %d extra character in first brackets\n",cur_file,  line_num);
                error=TRUE;
                return reg_word;
            }
            else if(reg_2!=INT_MIN)/*extra character in second brackets*/
            {
                printf("error: file %s line %d extra character in second brackets\n",cur_file,  line_num);
                error=TRUE;
                return reg_word;
            }
            else/*the second register*/
            {
                register_2[0]=rest_of_line[i];
                if(rest_of_line[i+1]!='\0')
                {
                    register_2[1]=rest_of_line[i+1];/*getting the entire register name*/
                    register_2[2]='\0';
                    i++;/*because we read another character*/
                    if(is_register(register_2)==-1)/*not a register name*/
                    {
                        printf("error: file %s line %d the matrix does not contain a valid register name\n",cur_file,  line_num);
                        error=TRUE;
                        return reg_word;
                    }
                    reg_2=TRUE;
                }
            }
        }
        else if(rest_of_line[i]==']')
        {
            if(!l_bracket1)/*missing first opening bracket*/
            {
                printf("error: file %s line %d missing first opening bracket\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
            else if(r_bracket1 && !l_bracket2)/*missing second opening bracket*/
            {
                printf("error: file %s line %d missing second opening bracket\n",cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
            else if(!num && strcmp(type, "data")==0)/*empty brackets*/
            {
                printf("error: file %s line %d empty brackets\n", cur_file, line_num);
                error=TRUE;
                return reg_word;
            }
            else if((reg_1==INT_MIN || (l_bracket2 && reg_2==INT_MIN)) && strcmp(type, "instructions")==0)/*empty brackets*/
            {
                printf("error: file %s line %d empty brackets\n",cur_file,  line_num);
                error=TRUE;
                return reg_word;
            }
            else if(!r_bracket1)/*first close bracket*/
            {
                r_bracket1=TRUE;
                num=FALSE;
            }
            else/*second close bracket*/
            {
                r_bracket2=TRUE;
                num=FALSE;
            }
        }
        else/*invalid character*/
        {
            printf("error: file %s line %d invalid character\n",cur_file, line_num);
            error=TRUE;
            return reg_word;
        }
    }
    if(r_bracket2==FALSE)
    {
        printf("error: file %s line %d missing second right bracket\n",cur_file, line_num);
        error=TRUE;
        return reg_word;
    }

    if(strcmp(type, "instructions")==0)/*instructions matrix*/
    {
        rest_of_line=rest_of_line+i;/*points to the place where the matrix content begins*/
        if(!line_ends_here(rest_of_line))/*there are extra characters after accessing the matrix*/
        {
            printf("error: file %s line %d has too many operands\n",cur_file,  line_num);
            error=TRUE;
        }
        return reg_to_word(is_register(register_1), is_register(register_2));
    }
    
    else/*data matrix*/
    {
        rest_of_line=rest_of_line+i;/*points to the place where the matrix content begins*/
        if((rest_of_line[0]!=' ') && rest_of_line[0]!='\t')/*the size of the matrix adjacent to the content that follows it*/
        {
            printf("error: file %s line %d the size of the matrix adjacent to the content that follows it\n", cur_file, line_num);
            error=TRUE;
            return reg_word;
        }
        else if(correct_commas(rest_of_line, line_num))/*the command punctuation is correct*/
        {
            char * word;
            int number;
            word=strtok(rest_of_line, " ,\t\n");/*word reception*/
            
            while(word!=NULL && values>0)
            {
                if((number=is_valid_number(word, line_num, MIN_NUM_10B, MAX_NUM_10B))==INT_MIN)/*the number isn't valid*/
                {
                    break;
                }
                else/*the number is valid*/
                {
                    char * binary_num;
                    number=atoi(word);
                    if(check_memory_full(line_num)==FALSE)
                    {
                        exit_flag=TRUE;
                        return reg_word;
                    }
                    binary_num= to_binary(number, BITS_OF_DATA_WORD);/*convert to binary*/
                    if(exit_flag)
                    {
                        return NULL;
                    }
                    fprintf(file_d, "%s\n",binary_num);/*inserting the number into the data file*/
                    free(binary_num);
                    dc++;/*data counter promotion*/
                    values--;/*to keep track of how many numbers we entered*/
                }
                word=strtok(NULL, " ,\t\n");/*get the next word*/
            }
            if(word==NULL || values==0)/*no error has been detected so far (there was no break)*/
            {
                if(word!=NULL && values==0)/*the loop stopped because the matrix is full, but there are more values*/
                {
                    printf("error: file %s line %d the number of values is greater than the size of the matrix\n",cur_file, line_num);
                    error=TRUE;
                    return reg_word;
                }
                
                else if(values>0)/*the number of values is less than the size of the matrix*/
                {
                    while(values>0)/*cells not initialized with values receive 0*/
                    {
                        if(check_memory_full(line_num)==FALSE)
                        {
                            exit_flag=TRUE;
                            return reg_word;
                        }
                        fprintf(file_d, "%s\n","0000000000");/*inserting the zeros into the data file*/
                        dc++;/*data counter promotion*/
                        values--;/*to keep track of how many numbers we entered*/
                    }
                }
            }    
        }
    }
    return reg_word;
}


/*the function checks if the string is a valid number*/
int is_valid_number(char * word, int line_num, int min_num, int max_num)
{
    char * p;/*pointer for strtol use*/
    int number;
    number=strtol(word, &p, 10);/*convert from string to long*/
    if(*p!='\0')/*the word contains non-numeric characters*/
    {
        printf("error: file %s line %d the number contains non-numeric characters\n",cur_file, line_num);
        error=TRUE;
        return INT_MIN;
    }
    if(number<min_num || number>max_num)/*if the number is out of range*/
    {
        printf("error: file %s line %d number out of range\n",cur_file, line_num);
        error=TRUE;
        return INT_MIN;
    }
    return (int)number;
}

/*the function accepts a decimal number and converts it to a binary string of the received length*/
char * to_binary(long number, int length)
{
    int i, j, num=number;
    char temp_num;
    char * binary_num=malloc(length + 1);
    if(!binary_num)
    {
        perror("malloc failed\n");
        exit_flag=TRUE;
        return NULL;
    }
    if(num<0)/*negative number*/
    {
        num=num*-1;/*we will make the number positive*/
    }
    if(num==0)
    {
        binary_num[0]='0';
        i=1;
    }
    for(i=0; num!=0; i++)/*building the binary number in reverse*/
    {
        if(num%2==0)
        {
            binary_num[i]='0';
        }
        else
        {
            binary_num[i]='1';
        }
        num=num/2;
    }
    for(j=i; j<length; j++)/*adding "leading" zeros as required*/
    {
        binary_num[j]='0';
    }
    i=0;
    j=length-1;
    while(i<j)/*reverse the binary string*/
    {
        temp_num=binary_num[i];
        binary_num[i]=binary_num[j];
        binary_num[j]=temp_num;
        i++;
        j--;
    }
    if(number<0)/*if the number received is negative, we will change the binary number to negative using the 2's complement method*/
    {
        for(i=length-1; i>=0 && binary_num[i]!='1'; i--);/*finding the first 1 on the right*/
        for(j=i-1; j>=0; j--)/*1 is replaced by 0, 0 by 1*/
        {
            if(binary_num[j]=='1')
            {
                binary_num[j]='0';
            }
            else
            {
                binary_num[j]='1';
            }
        }
    }
    binary_num[length]='\0';
    return binary_num;
}

/*the function checks whether the received word meets label validity rules*/
int label_syntax(char * word, int line_num)
{
    int i;

    if(strcmp(word, "")==0)/*empty label*/
    {
        printf("error: file %s line %d number out of range\n",cur_file, line_num);
        return FALSE;
    }
    if(strlen(word)>30)/*label length exceeds the allowed length*/
    {
        printf("error: file %s line %d label length exceeds the allowed length\n",cur_file, line_num);
        return FALSE;
    }

    if((!isalpha((unsigned char)word[0])))/*the first character must be a letter*/
    {
        printf("error: file %s line %d invalid label, starts with a non-letter character\n", cur_file,line_num);
        return FALSE;
    }
    for(i=1; word[i]!='\0'; i++)/*checking the correctness of the label characters*/
    {
        if(!isalpha(word[i]) && !isdigit(word[i]))/*a label can only contain letters or numbers*/
        {
            printf("error: file %s line %d invalid label, contains a character that is neither a letter nor a number\n", cur_file, line_num);
            return FALSE;
        }
    }
    return TRUE;
}

/*the function checks whether the received string is a valid string*/
char * is_correct_string(char * word, int line_num)
{
    int i, j, k;
    char * str_content=malloc(MAX_LINE);
    if(!str_content)/*allocation failed*/
    {
        perror("malloc failed");
		exit_flag=TRUE;
		return NULL;
    }
    for(i=strlen(word)-1; i>=0; i--)/*checking whether a " bracket exists and is not followed by other characters*/
    {
        if(i==0)/*no quotation marks were found until the first character*/
        {
            printf("error: file %s line %d the argument received by the '.string' prompt is not enclosed in quotes\n",cur_file,  line_num);
            error=TRUE;
            return NULL;
        }
        if(word[i]=='"')
        {
            i--;
            break;
        }
        if(!isspace(word[i]))/*the argument is not enclosed in quotes*/
        {
            printf("error: file %s line %d the argument received by the '.string' prompt is not enclosed in quotes\n",cur_file,  line_num);
            error=TRUE;
            return NULL;
        }
    }
    for(j=0; j<=i; j++)/*check from the beginning to the closing " if there is an opening " and if there is an unnecessary character before it*/
    {
        if((j==i) && word[j]!='"')/*the last character before the closing quote is not an opening quote*/
        {
            printf("error: file %s line %d the argument received by the '.string' prompt is not enclosed in quotes\n",cur_file,  line_num);
            error=TRUE;
            return NULL;
        }
        if(word[j]=='"')
        {
            j++;
            break;
        }
        if(!isspace(word[j]))/*there is a character that is not part of the string*/
        {
            printf("error: file %s line %d there is a character that is not part of the string\n",cur_file,  line_num);
            error=TRUE;
            return NULL;
        }
    }
    if(i<j)/*empty string*/
    {
        str_content[0]='\0';
        return str_content;
    }
    for(k=j; k<=i; k++)/*checking whether there are valid characters inside the surrounding quotation marks*/
    {
        if(!isprint((unsigned char)word[k]))/*if the string contains non-printable characters*/
        {
            printf("error: file %s line %d the string contains non-printable characters\n",cur_file,  line_num);
            error=TRUE;
            return NULL;
        }
        str_content[k-j]=word[k];/*copy the string without the surrounding quotes*/
    }
    str_content[k-j]='\0';
    return str_content;
}

/*the function checks if the received word is the name of a label*/
int find_symbol(char * word, symbols * symbols_table, int current_symbol_count)
{
    int i;
    for(i=0; i<current_symbol_count; i++)
    {
        if(strcmp(symbols_table[i].symbol_name, word)==0)/*a label with the same name was found*/
        {
            return TRUE;
        }
    }
    return FALSE;
}


/*the function checks if there is room in the program memory for another word*/
int check_memory_full(int line_num)
{
    if(dc+ic+1==256)/*memory full*/
    {
        printf("error: file %s line %d 156 memory cells are full\n", cur_file, line_num);
        return FALSE;
    }
    return TRUE;
}

/*the function checks whether the received operand is suspected of being a matrix*/
int is_mat(char* operand) 
{
    if (operand == NULL) 
    {
        return FALSE;
    }

    if (strchr(operand, '[') != NULL) 
    {
        return TRUE;
    }
    return FALSE;
}



/* function to calculate the number of lines needed for an instruction */
int num_of_lines(int first_operand, int second_operand, int operand_num)
{
    int num_of_lines = 1; /*initialize number of lines to 1*/
    if(operand_num == 0) /*if there are no operands, we need only one line*/
    {
        return num_of_lines;
    }
    else
    {
        if(operand_num == 1) /*if there is one operand, we need two lines at least*/
        {
            num_of_lines += 1;
            if(first_operand == MATRIX_ADD) /*if the operand is a mat addressing mode, we need two extra lines*/
            {
                num_of_lines += 1;
            }
            return num_of_lines;
        }
        else 
        {
            if(operand_num == 2) /*if there are two operands, we need three lines at least*/
            {
                if(first_operand == REGISTER_ADD && second_operand == REGISTER_ADD)
                {
                    num_of_lines ++; /*if both operands are register addressing mode, we need only one extra line*/
                }
                else
                {
                    num_of_lines += 2; /*any other addressing mode combination requires two extra lines at least*/
                    if(first_operand == MATRIX_ADD) /*if the operand is a mat addressing mode, we need two extra lines*/
                    {
                        num_of_lines ++; /*we need one extra line for the mat addressing mode*/
                    }
                    if( second_operand == MATRIX_ADD) /*if the operand is a mat addressing mode, we need two extra lines*/
                    {
                        num_of_lines ++; /*we need one extra line for the mat addressing mode*/
                    }
                }
            }
        }
        return num_of_lines;
    }
}

/*the function checks if the recieved operand is a valid number*/
int immediate(char * operand, int line_num)
{
	int num=0;/*used for turning char to int*/
	int is_neg=FALSE;
	int has_num=FALSE;
	char *ptr = operand;
	if(*ptr=='#')
	{
		ptr++;
	}
	if(*ptr=='-')
	{
		is_neg=TRUE;
		ptr++;
	}
	while (*ptr != '\0') 
	{
		has_num=TRUE;
		if(!isdigit(*ptr))
		{
			printf("error: file %s line %d operand is not a number\n", cur_file, line_num);
			return INT_MIN; /*return false if the operand is not a number or is out of range*/
		}
       		num = num * 10 + (*ptr - '0');
		ptr++;

	}
	if(!has_num || num < MIN_NUM_10B || num > MAX_NUM_10B) /*if the operand is not a number or is out of range*/
	{
		printf("error: file %s line %d number is out of range\n", cur_file, line_num);
		return INT_MIN; /*return false if the operand is not a number or is out of range*/
	}
	if(is_neg)
	{
		num*=-1;
	}
	return num;
}

/*the function accepts the names of two registers and returns a binary word of their names*/
char * reg_to_word(int reg_1, int reg_2)
{
	char era [REG_SIZE]="00";    
	char *binary_reg1=NULL ;
	char *binary_reg2=NULL;
	char * reg_word=NULL;
	binary_reg1 = to_binary(reg_1, 4);
	if (binary_reg1==NULL)
	{
		exit_flag=TRUE;
		return NULL; /* to_binary failed, exit_flag is set*/
	}
	binary_reg2 = to_binary(reg_2, 4);
	if (binary_reg2==NULL)
	{
		free(binary_reg1);
		exit_flag=TRUE;
		return NULL; /* to_binary failed, exit_flag is set*/
	}
	reg_word = malloc(WORD_SIZE+1); /*allocate memory for reg_word*/
	if (reg_word==NULL)
	{
		perror("malloc failed");
		free(binary_reg1); /* free the first allocated memory*/
		free(binary_reg2);
		exit_flag=TRUE;
		return NULL;
	}
	strcpy(reg_word, binary_reg1); /*initialize reg_word*/
	strcat(reg_word, binary_reg2); /*concatenate the second register to the first one*/
	strcat(reg_word, era); /*add "00" to the end of the register word*/
	strcat(reg_word, "\n"); /*add \n to the end of the register word*/
	reg_word[WORD_SIZE] = '\0';
	/* Free the intermediate binary strings*/
	free(binary_reg1);
	free(binary_reg2);
	return reg_word; /*return the register word*/
}

/*the function checks if the recieved word is a register name*/
int is_register(char * word)
{
	/*check if the word is a register*/
	if(strlen(word) == REG_SIZE-1)/*in case when sent by matrix, without comma, if it was sent by operand with comma correct_comas deals with it*/
	{ 

		if(word[0] == 'r' && isdigit(word[1]) && (word[1] >= '0' && word[1] <= '7'))
		{
			return word[1]-'0'; /*if it is a register return register value*/
		}
		else /*if it is not a register return false*/
		{
			return -1;
		}
		
	}
	else 
	{
		if(strlen(word) == REG_SIZE)/*in case when sent by operand, with comma*/
		{

			if(word[0] == 'r' && isdigit(word[1]) && (word[1] >= '0' && word[1] <= '7') && word[2]==',')
			{
				return word[1]-'0'; /*if it is a register return register value*/
			}
			else /*if it is not a register return false*/
			{
				return -1;
			}
		}
		else/*too long or too short*/
		{

			return -1;
		}
	}
	return -1; /*if it is not a register return false*/
}

/*the function checks whether the sent line does not contain non-whitespace characters*/
int line_ends_here(char * rest_of_line)
{
	if(rest_of_line ==NULL)
	{
		return TRUE;
	}
	while (*rest_of_line != '\0') 
	{
		if(!isspace((unsigned char)*rest_of_line)) 
		{
			return FALSE;
		}
		rest_of_line++;
	}
	return TRUE;
}

/*the function combines the contents of two files into one file*/
void combine_files(FILE * file_i, FILE * file_d)
{
    char line[LINE_LENGTH+2]={0};
    fseek(file_d, 0, SEEK_SET);
    fseek(file_i, 0, SEEK_END);
    while((fgets(line, LINE_LENGTH+2, file_d)!=NULL))
    {
        fprintf(file_i, "%s",line);
    }
}



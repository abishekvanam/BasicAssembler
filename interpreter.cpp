#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>

//***************************************************

void generateIntermediateTable(int *start_pos);

//***************************************************

int symbol_table_length = 20;
int label_table_length = 10;
int intermediate_table_length = 20;
int input_file_length;
int instruction_count_in_intermediate = 1;
int symbol_table_address = 0;
int symbol_table_pointer = 0;
int intermediate_table_pointer = 0;
int label_table_pointer = 0;
int top_of_ifStack = -1;


int *jump_check_index = (int*)malloc(sizeof(int));

struct symbols_table
{
	char *name;
	int address;
	int size;
	int is_readonly;
};

struct labels_table
{
	char *label_name;
	int address;
};

struct intermediates_table
{
	int instruction_number;
	int opcode;
	int p1, p2, p3, p4, p5;
};

char **instructions_table;

int *registers;//= AX, BX, CX, DX, EX, FX, GX;

int *memory_table;

int *ifStack;

struct symbols_table** symbol_table;

struct labels_table** label_table;

struct intermediates_table** intermediate_table;

FILE* open_file(char *fname)
{
	FILE *fp = fopen(fname, "r");
	if (fp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	else
	{
		return fp;
	}
}

void fileclose(FILE *fp)
{
	fclose(fp);
}

int numOfRows(char *fname)
{
	/*
	FILE *fp = fopen(fname, "r");//open_file(fname);
	int len = 0;
	char str[500];
	while (fgets(str, 500, fp) != NULL)
	{
		len++;
	}
	fseek(fp, 0, SEEK_SET);
	return len;
	*/

	FILE *fp;
	int count = 0;  // Line counter (result)
	char c;
	// Open the file
	fp = fopen(fname, "r");

	// Check if file exists
	if (fp == NULL)
	{
		printf("Could not open file %s", fname);
		return 0;
	}

	// Extract characters from file and store in character c
	for (c = getc(fp); c != EOF; c = getc(fp))
		if (c == '\n') // Increment count if this character is newline
			count = count + 1;

	fclose(fp);
	return count;
}


void initialize()
{

	*jump_check_index = 0;

	registers = (int*)malloc(8 * sizeof(int));

	intermediate_table = (struct intermediates_table**)malloc(intermediate_table_length*sizeof(struct intermediates_table*));

	label_table = (struct labels_table**)malloc((label_table_length)*sizeof(struct labels_table*));

	symbol_table = (struct symbols_table**)malloc(symbol_table_length * sizeof(struct symbols_table*));

	memory_table = (int*)malloc(20 * sizeof(int));

	ifStack = (int*)malloc(30 * sizeof(int));
}

void loadInstructions(FILE* fp)
{
	instructions_table = (char**)malloc(input_file_length*sizeof(char*));
	char *str = (char*)malloc(500 * sizeof(char));
	int i = 0;
	while (fgets(str, 500, fp))
	{
		instructions_table[i] = str;
		str = (char*)malloc(500 * sizeof(char));
		//strcpy(instructions_table[i], str);
		//instructions_table[i++] = str;
		i++;
	}
}


void generateSymbolTable(int *start_pos)
{
	int i = 0;
	char c;
	char *instruction_from_file = (char*)malloc(100 * sizeof(char));
	strcpy(instruction_from_file,instructions_table[i]);
	while (/*(strcmp(instruction_from_file, "START:") !=0)&&*/i<input_file_length)
	{
		instruction_from_file = (char*)malloc(100 * sizeof(char));
		strcpy(instruction_from_file, instructions_table[i]);
		i++;
		char *token = (char*)malloc(100 * sizeof(char));
		char *dummy_instruction = instruction_from_file;
		token=strtok(instruction_from_file, " ");


		if (strcmp(token, "DATA")==0)
		{
			token = strtok(NULL, "\n");
			struct symbols_table *symbol_table_entry = (struct symbols_table*)malloc(sizeof(struct symbols_table));
			symbol_table_entry->address = symbol_table_address;
			symbol_table_entry->is_readonly = 0;
			
			if (strlen(token) == 1)
			{

				symbol_table_entry->name = token;

				symbol_table_entry->size = 1;
				
				symbol_table_address += 1;
			
			}

			else if (strlen(token) > 1)
			{
				char *ck = (char*)malloc(1*sizeof(char));
				int index = 0;
				while (token[index] != '[')
				{
					ck[index] = token[index];
					index++;
				}
				ck[index] = '\0';
				symbol_table_entry->name = ck;// token[0];
					int j = 0,flag=0,num=0;
					while (j < strlen(token))
					{
						if (token[j] == '[')
						{
							flag = 1;
						}
						else if (flag == 1)
						{
							if (token[j] >= '0'&&token[j] <= '9')
							{
								num = num*10+(token[j] - '0');
							}
						}
						if (token[j] == ']')
							break;
						j++;
					}

					symbol_table_entry->size = num;
					symbol_table_address += num;
			}
			symbol_table[symbol_table_pointer] = symbol_table_entry;
			symbol_table_pointer++;
		}//Upto here for DATA!

		else if (strcmp(token, "CONST")==0)
		{
			token = strtok(NULL, "=");
			struct symbols_table *symbol_table_entry = (struct symbols_table*)malloc(sizeof(struct symbols_table));
			symbol_table_entry->address = symbol_table_address;
			symbol_table_entry->is_readonly = 1;
			symbol_table_entry->name = token;
			token = strtok(NULL, " ");
			int num = atoi(token);
			symbol_table_entry->size = 1;
			memory_table[symbol_table_address] = num;
			symbol_table_address += 1;
			symbol_table[symbol_table_pointer] = symbol_table_entry;
			symbol_table_pointer++;
		}
		else
		{
			break;
		}
		*start_pos = *start_pos + 1;
		
	}
	//*start_pos = *start_pos + 1;
}

void printSymbolTable()
{
	printf("Name\tAddress\tsize\tis_readonly\n");
	for (int i = 0; i < symbol_table_pointer; i++)
	{
		printf("%s\t%d\t%d\t%d\n\n", symbol_table[i]->name, symbol_table[i]->address, symbol_table[i]->size, symbol_table[i]->is_readonly);
	}
}





//**********************************************************************************************************

struct intermediates_table *intermediate_table_entry;



char *ignoreSpaces(char *str)
{
	char *c = (char*)malloc((strlen(str)+1)*sizeof(char));
	int index = 0;
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == ' '||str[i]=='\n'||str[i]=='\t')
		{
			continue;
		}
		c[index++] = str[i];
	}
	c[index] = '\0';
	return c;
}


int checkIfRegister(char *ch)
{
	char regis[8][5] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
	char *c = ignoreSpaces(ch);
	for (int i = 0; i < 8; i++)
	{
		if (strcmp(c, regis[i]) == 0)
		{
			return 1;//Found
		}
	}
	return 0;//Not Found
}

int getRegisterIndex(char *token)
{
	char *c = ignoreSpaces(token);

	if (strcmp(c, "AX") == 0)
	{
		return 0;
	}
	else if (strcmp(c, "BX") == 0)
	{
		return 1;
	}
	else if (strcmp(c, "CX") == 0)
	{
		return 2;
	}
	else if (strcmp(c, "DX") == 0)
	{
		return 3;
	}
	else if (strcmp(c, "EX") == 0)
	{
		return 4;
	}
	else if (strcmp(c, "FX") == 0)
	{
		return 5;
	}
	else if (strcmp(c, "GX") == 0)
	{
		return 6;
	}
	else if (strcmp(c, "HX") == 0)
	{
		return 7;
	}
}


int getSymbolIndex(char *symbol)
{
	for (int i = 0; i < symbol_table_pointer; i++)
	{
		if (strcmp(ignoreSpaces(symbol_table[i]->name), ignoreSpaces(symbol)) == 0)
		{
			return symbol_table[i]->address;
		}
	}
	return -1;
}


int getOperatorOpcode(char *ch)
{
	if (strcmp(ch, "EQ")==0)
	{
		return 8;
	}
	else if (strcmp(ch, "LT")==0)
	{
		return 9;
	}
	else if (strcmp(ch, "GT")==0)
	{
		return 10;
	}
	else if (strcmp(ch, "LTEQ")==0)
	{
		return 11;
	}
	else if (strcmp(ch, "GTEQ")==0)
	{
		return 12;
	}
}


int getLabelValue(char *ch)
{
	for (int i = 0; i < label_table_pointer; i++)
	{
		if (strcmp(label_table[i]->label_name, ch) == 0)
		{
			return label_table[i]->address;
		}
	}
	return -1;
}

void read(char *token)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 14;//From table in ppt
	intermediate_table_entry->p1 = getRegisterIndex(token);
	intermediate_table_entry->p2 = -1;
	intermediate_table_entry->p3 = -1;
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

void jump(char *token)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 6;//From table in ppt
	intermediate_table_entry->p1 = getLabelValue(ignoreSpaces(token));
	intermediate_table_entry->p2 = -1;
	intermediate_table_entry->p3 = -1;
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

int checkIfArray(char *token)
{
	int num=1;
	for (int i = 0; i < strlen(token); i++)
	{
		if (token[i] == '[')
		{
			i++;
			while (1)
			{
				if (token[i] >= '0'&&token[i] <= '9')
				{
					num = num*(token[i] - '0');
				}
				i++;
				if (token[i] == ']')
					return num;
			}
		}
	}
	return -1;
}

void movRToM(char *token1, char* token2)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));
	
	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 1;

	intermediate_table_entry->p3 = -1;
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;
	//A,B,C
	int ret = checkIfArray(token1);
	if (ret == -1)
	{

		intermediate_table_entry->p1 = getSymbolIndex(token1);
		intermediate_table_entry->p2 = getRegisterIndex(token2);
	}
	else
	{
		token1 = strtok(token1, "[");
		intermediate_table_entry->p1 = getSymbolIndex(token1)+ret;
		intermediate_table_entry->p2 = getRegisterIndex(token2);
	}
	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}




void movMToR(char *token1, char* token2)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 2;



	intermediate_table_entry->p1 = getRegisterIndex(token1);
	intermediate_table_entry->p2 = getSymbolIndex(token2);
	intermediate_table_entry->p3 = -1;
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}




void add(char* token1, char* token2, char* token3)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 3;


	intermediate_table_entry->p1 = getRegisterIndex(token1);
	intermediate_table_entry->p2 = getRegisterIndex(token2);
	intermediate_table_entry->p3 = getRegisterIndex(token3);
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

void sub(char* token1, char* token2, char* token3)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 4;


	intermediate_table_entry->p1 = getRegisterIndex(token1);
	intermediate_table_entry->p2 = getRegisterIndex(token2);
	intermediate_table_entry->p3 = getRegisterIndex(token3);
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

int containsChar(char* str, char ch)
{
	for (int i = 0; i < strlen(str); i++)
	{
		if (str[i] == ch)
		{
			return 1;
		}
	}
	return 0;
}

void push(int data)
{
	ifStack[++top_of_ifStack] = data;//intermediate_table_pointer;
}

void pop()
{
	ifStack[top_of_ifStack--] = -1;
}

void ifStatement(char* token1, char* token2, char* token3)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	intermediate_table_entry->opcode = 7;


	intermediate_table_entry->p1 = getRegisterIndex(token1);
	intermediate_table_entry->p2 = getRegisterIndex(token3);
	intermediate_table_entry->p3 = getOperatorOpcode(token2); //getRegisterIndex(token2);
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	//ifStack[++top_of_ifStack] = instruction_count_in_intermediate;
	push(intermediate_table_pointer);


	instruction_count_in_intermediate += 1;
	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

void elseStatement()
{
	int if_position = ifStack[top_of_ifStack];

	intermediate_table[if_position]->p4 = instruction_count_in_intermediate+1;//intermediate_table_pointer+1;//+1 is to point to the current instruction

	pop();

	push(intermediate_table_pointer);

	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	intermediate_table_entry->opcode = 6;


	intermediate_table_entry->p1 = -1;
	intermediate_table_entry->p2 = -1;
	intermediate_table_entry->p3 = -1; //getRegisterIndex(token2);
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	instruction_count_in_intermediate += 1;
	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

void endifStatement()
{
	int tos_value = ifStack[top_of_ifStack];

	if (intermediate_table[tos_value]->opcode == 7)//To check if endif is after if(else is not present)
	{
		intermediate_table[tos_value]->p4 = intermediate_table_pointer;
	}
	else
	{
		intermediate_table[tos_value]->p1 = instruction_count_in_intermediate+1;//intermediate_table_pointer;
	}

	pop();

	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	intermediate_table_entry->opcode = 0;


	intermediate_table_entry->p1 = -1;
	intermediate_table_entry->p2 = -1;
	intermediate_table_entry->p3 = -1; //getRegisterIndex(token2);
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	//ifStack[++top_of_ifStack] = instruction_count_in_intermediate;
	//push(instruction_count_in_intermediate);
	instruction_count_in_intermediate += 1;
	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;

}

void print(char *token)
{
	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));


	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
	instruction_count_in_intermediate += 1;
	intermediate_table_entry->opcode = 13;//From table in ppt
	intermediate_table_entry->p1 = getRegisterIndex(token);
	intermediate_table_entry->p2 = -1;
	intermediate_table_entry->p3 = -1;
	intermediate_table_entry->p4 = -1;
	intermediate_table_entry->p5 = -1;

	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
	intermediate_table_pointer++;
}

void generateIntermediateTable(int *start_pos)
{
	int start = *start_pos+1;
	//int i = 0;
	char c;
	char *instruction_from_file = (char*)malloc(100 * sizeof(char));
	strcpy(instruction_from_file, instructions_table[start]);

	while (start < input_file_length-1)
	{
		instruction_from_file = (char*)malloc(100 * sizeof(char));
		strcpy(instruction_from_file, instructions_table[start]);
		start++;
		char *token = (char*)malloc(100 * sizeof(char));
		char *dummy_instruction = instruction_from_file;
		token = ignoreSpaces(strtok(instruction_from_file, " "));


		if (strcmp(token, "READ") == 0)
		{
			token = strtok(NULL, "" );
			read(token);


			
		}//End of READ

		else if (strcmp(token, "MOV") == 0)
		{
			char *t1 = (char*)malloc(2 * sizeof(char));
			char *t2 = (char*)malloc(2 * sizeof(char));
			t1 = strtok(NULL, ",");
			t2 = strtok(NULL, "\n");
			if (checkIfRegister(t2) == 1)
			{
				movRToM(t1, t2);//Left is destination, Right is source
			}
			else
			{
				movMToR(t1, t2);
			}
		}//End of MOV

		else if (strcmp(token, "ADD") == 0)
		{
		
			char *t1 = (char*)malloc(2 * sizeof(char));
			char *t2 = (char*)malloc(2 * sizeof(char));
			char *t3 = (char*)malloc(2 * sizeof(char));
			t1 = strtok(NULL, ",");
			t2 = strtok(NULL, ",");
			t3 = strtok(NULL, "\n");
			add(t1, t2, t3);
		
		}//End of ADD

		else if (strcmp(ignoreSpaces(token), "SUB") == 0)
		{

			//token = strtok(NULL, " ");
			char *t1 = (char*)malloc(2 * sizeof(char));
			char *t2 = (char*)malloc(2 * sizeof(char));
			char *t3 = (char*)malloc(2 * sizeof(char));
			t1 = strtok(NULL, ",");
			t2 = strtok(NULL, ",");
			t3 = strtok(NULL, "\n");
			sub(t1, t2, t3);

		}//End of SUB

		else if (containsChar(token,':')==1)
		{
			struct labels_table *label_table_record = (struct labels_table*)malloc(sizeof(struct labels_table));
			label_table_record->address = instruction_count_in_intermediate;
			token = strtok(token, ":");
			token = ignoreSpaces(token);
			label_table_record->label_name = token;
			label_table[label_table_pointer] = label_table_record;
			label_table_pointer += 1;

		}//End of label

		else if (strcmp(token, "JUMP")==0)
		{
			token = ignoreSpaces(strtok(NULL, "\n"));
			jump(token);

		}//End of JUMP

		else if(strcmp(token, "IF") == 0)
		{
			char *t1 = (char*)malloc(2 * sizeof(char));
			char *t2 = (char*)malloc(2 * sizeof(char));
			char *t3 = (char*)malloc(2 * sizeof(char));
			t1 = strtok(NULL, " ");
			t2 = strtok(NULL, " ");
			t3 = strtok(NULL, " ");
			ifStatement(ignoreSpaces(t1), ignoreSpaces(t2), ignoreSpaces(t3));
		}
		else if (strcmp(token, "PRINT")==0)
		{
			token = strtok(NULL, "");
			print(token);

		}
		else if (strcmp(token, "ELSE") == 0)
		{
			elseStatement();
		}
		else if (strcmp(token, "ENDIF") == 0)
		{
			endifStatement();
		}

	}
}

void printIntermediateTable()
{
	printf("InsNumber\tOpCode\tp1\tp2\tp3\tp4\tp5\n");
	for (int i = 0; i <intermediate_table_pointer; i++)
	{
		printf("%d\t\t%d\t%d\t", intermediate_table[i]->instruction_number, intermediate_table[i]->opcode, intermediate_table[i]->p1);
		if (intermediate_table[i]->p2 != -1)
		{
			printf("%d\t", intermediate_table[i]->p2);
		}
		else
		printf("\t");
		if (intermediate_table[i]->p3 != -1)
		{
			printf("%d\t", intermediate_table[i]->p3);
		}
		else
		printf("\t");
		if (intermediate_table[i]->p4 != -1)
		{
			printf("%d\t", intermediate_table[i]->p4);
		}
		else
		printf("\t");
		if (intermediate_table[i]->p5 != -1)
		{
			printf("%d\t", intermediate_table[i]->p5);
		}
		else
		printf("\t\n\n");
	}
}



//***************************************************************************************************

//COMPUTATIONS



void computeMovRToM(int intermediate_table_index)
{
	memory_table[symbol_table[intermediate_table[intermediate_table_index]->p1]->address] = registers[intermediate_table[intermediate_table_index]->p1];
}

void computeMovMToR(int intermediate_table_index)
{
	registers[intermediate_table[intermediate_table_index]->p1]=memory_table[intermediate_table[intermediate_table_index]->p2];
	//registers[intermediate_table[intermediate_table_index]->p1];
}


void computeAdd(int intermediate_table_index)
{
	registers[intermediate_table[intermediate_table_index]->p1] = registers[intermediate_table[intermediate_table_index]->p2] + registers[intermediate_table[intermediate_table_index]->p3];
	//printf("Sum is : %d\n", registers[intermediate_table[intermediate_table_index]->p1]);
}


void computeSub(int intermediate_table_index)
{
	registers[intermediate_table[intermediate_table_index]->p1] = registers[intermediate_table[intermediate_table_index]->p2] - registers[intermediate_table[intermediate_table_index]->p3];
	//printf("Difference is : %d", registers[intermediate_table[intermediate_table_index]->p1]);
}


void computeMul(int intermediate_table_index)
{
	registers[intermediate_table[intermediate_table_index]->p1] = registers[intermediate_table[intermediate_table_index]->p2] * registers[intermediate_table[intermediate_table_index]->p3];
	//printf("Multiplication is : %d", registers[intermediate_table[intermediate_table_index]->p1]);
}


int computeJump(int intermediate_table_index)
{
	return intermediate_table[intermediate_table_index]->p1-1;
}


void computeRead(int intermediate_table_index)
{
	printf("Enter the value to put in the register:");
	scanf("%d", &registers[intermediate_table[intermediate_table_index]->p1]);
}


void computePrint(int intermediate_table_index)
{
	printf("\n%d\n", registers[intermediate_table[intermediate_table_index]->p1]);
}

int computeIf(int intermediate_table_index)
{
	int ret = 0;
	if (intermediate_table[intermediate_table_index]->p3 == 8)
	{
		if (registers[intermediate_table[intermediate_table_index]->p1] == registers[intermediate_table[intermediate_table_index]->p2])
		{
			ret = 1;
		}
	}
	else if (intermediate_table[intermediate_table_index]->p3 == 9)
	{
		if (registers[intermediate_table[intermediate_table_index]->p1] < registers[intermediate_table[intermediate_table_index]->p2])
		{
			ret = 1;
		}
	}
	else if (intermediate_table[intermediate_table_index]->p3 == 10)
	{
		if (registers[intermediate_table[intermediate_table_index]->p1] > registers[intermediate_table[intermediate_table_index]->p2])
		{
			ret = 1;
		}
	}
	else if (intermediate_table[intermediate_table_index]->p3 == 11)
	{
		if (registers[intermediate_table[intermediate_table_index]->p1] <= registers[intermediate_table[intermediate_table_index]->p2])
		{
			ret = 1;
		}
	}
	else if (intermediate_table[intermediate_table_index]->p3 == 12)
	{
		if (registers[intermediate_table[intermediate_table_index]->p1] >= registers[intermediate_table[intermediate_table_index]->p2])
		{
			ret = 1;
		}
	}
	return ret;


}


void computeEndIf()
{
	//printf("\n\n");
	/*for (int i = 0; i < 8; i++)
	{
		printf("%d\t",registers[i]);
	}*/
}

void computeIntermediateInstructions()//int intermediate_table_index)
{
	for (int i = 0; i < intermediate_table_pointer; i++)
	{

		if (intermediate_table[i]->opcode == 1)
		{
			computeMovRToM(i);
		}
		else if (intermediate_table[i]->opcode == 2)
		{
			computeMovMToR(i);
		}
		else if (intermediate_table[i]->opcode == 3)
		{
			computeAdd(i);
		}
		else if (intermediate_table[i]->opcode == 4)
		{
			computeSub(i);
		}
		else if (intermediate_table[i]->opcode == 5)
		{
			computeMul(i);
		}
		else if (intermediate_table[i]->opcode == 6)
		{
			i = computeJump(i)-1;
		}
		else if (intermediate_table[i]->opcode == 7)
		{
			if (computeIf(i) == 0)
			{
				i = intermediate_table[i]->p4-1;
			}
		}
		else if (intermediate_table[i]->opcode == 13)
		{
			computePrint(i);
		}
		else if (intermediate_table[i]->opcode == 14)
		{
			computeRead(i);
		}
		else if (intermediate_table[i]->opcode == 0)
		{
			computeEndIf();//Just prints all the registers values
		}
	}
}





void printFile()
{
	printf("\n\n");
	for (int i = 0; i < input_file_length-1; i++)
	{
		printf("%s\n", instructions_table[i]);
	}
}




//*****************************************************************************************************



void main()
{
	initialize();
	printf("Hello\n\n");
	FILE *fp = fopen("instructions.txt", "r");// open_file("instructions.txt");
	input_file_length = numOfRows("instructions.txt")+1;

	loadInstructions(fp);
	
	fileclose(fp);
	
	int *start_pos = (int*)malloc(sizeof(int));
	*start_pos = 0;
	printf("\n\nFile is :\n\n");
	printFile();
	printf("\n\n");
	generateSymbolTable(start_pos);
	
	printSymbolTable();

	generateIntermediateTable(start_pos);

	printIntermediateTable();

	
	computeIntermediateInstructions();
	

	system("pause");
}

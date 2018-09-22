//#include "interpreter.cpp"
//struct intermediates_table *intermediate_table_entry;
//
//
//
//char *ignoreSpaces(char *str)
//{
//	char *c = (char*)malloc(strlen(str)*sizeof(char));
//	int index = 0;
//	for (int i = 0; i < strlen(str); i++)
//	{
//		if (str[i] == ' ')
//		{
//			continue;
//		}
//		c[index++] = str[i];
//	}
//	c[index] = '\0';
//	return c;
//}
//
//
//int checkIfRegister(char *ch)
//{
//	char regis[8][5] = { "AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX" };
//	char *c = ignoreSpaces(ch);
//	for (int i = 0; i < 8; i++)
//	{
//		if (strcmp(c, regis[i]) == 0)
//		{
//			return 1;//Found
//		}
//	}
//	return 0;//Not Found
//}
//
//int getRegisterIndex(char *token)
//{
//	char *c = ignoreSpaces(token);
//
//	if (strcmp(c, "AX") == 0)
//	{
//		return 0;
//	}
//	else if (strcmp(c, "BX") == 0)
//	{
//		return 1;
//	}
//	else if (strcmp(c, "CX") == 0)
//	{
//		return 2;
//	}
//	else if (strcmp(c, "DX") == 0)
//	{
//		return 3;
//	}
//	else if (strcmp(c, "EX") == 0)
//	{
//		return 4;
//	}
//	else if (strcmp(c, "FX") == 0)
//	{
//		return 5;
//	}
//	else if (strcmp(c, "GX") == 0)
//	{
//		return 6;
//	}
//	else if (strcmp(c, "HX") == 0)
//	{
//		return 7;
//	}
//}
//
//
//
//int getSymbolIndex(char *symbol)
//{
//	for (int i = 0; i < symbol_table_pointer; i++)
//	{
//		if (strcmp(ignoreSpaces(symbol_table[i]->name), ignoreSpaces(symbol)) == 0)
//		{
//			return symbol_table[i]->address;
//		}
//	}
//	return -1;
//}
//
//
//void read(char *token)
//{
//	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));
//
//
//	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
//	instruction_count_in_intermediate += 1;
//	intermediate_table_entry->opcode = 14;//From table in ppt
//	intermediate_table_entry->p1 = getRegisterIndex(token);
//	intermediate_table_entry->p2 = -1;
//	intermediate_table_entry->p3 = -1;
//	intermediate_table_entry->p4 = -1;
//	intermediate_table_entry->p5 = -1;
//
//	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
//	intermediate_table_pointer++;
//}
//
//
//
//void movRToM(char *token1, char* token2)
//{
//	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));
//
//	//A,B,C
//	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
//	instruction_count_in_intermediate += 1;
//	intermediate_table_entry->opcode = 1;
//
//
//	intermediate_table_entry->p1 = getSymbolIndex(token1);
//	intermediate_table_entry->p2 = getRegisterIndex(token2);
//	intermediate_table_entry->p3 = -1;
//	intermediate_table_entry->p4 = -1;
//	intermediate_table_entry->p5 = -1;
//
//	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
//	intermediate_table_pointer++;
//}
//
//
//void movMToR(char *token1, char* token2)
//{
//	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));
//
//
//	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
//	instruction_count_in_intermediate += 1;
//	intermediate_table_entry->opcode = 2;
//
//
//	intermediate_table_entry->p1 = getRegisterIndex(token1);
//	intermediate_table_entry->p2 = getSymbolIndex(token2);
//	intermediate_table_entry->p3 = -1;
//	intermediate_table_entry->p4 = -1;
//	intermediate_table_entry->p5 = -1;
//
//	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
//	intermediate_table_pointer++;
//}
//
//
//
//
//void add(char* token1, char* token2, char* token3)
//{
//	intermediate_table_entry = (struct intermediates_table *)malloc(sizeof(struct intermediates_table));
//
//
//	intermediate_table_entry->instruction_number = instruction_count_in_intermediate;
//	instruction_count_in_intermediate += 1;
//	intermediate_table_entry->opcode = 3;
//
//
//	intermediate_table_entry->p1 = getRegisterIndex(token1);
//	intermediate_table_entry->p2 = getRegisterIndex(token2);
//	intermediate_table_entry->p3 = getRegisterIndex(token3);
//	intermediate_table_entry->p4 = -1;
//	intermediate_table_entry->p5 = -1;
//
//	intermediate_table[intermediate_table_pointer] = intermediate_table_entry;
//	intermediate_table_pointer++;
//}
//
//
//
//
//void generateIntermediateTable(int *start_pos)
//{
//	int start = *start_pos;
//	//int i = 0;
//	char c;
//	char *instruction_from_file = (char*)malloc(100 * sizeof(char));
//	strcpy(instruction_from_file, instructions_table[start]);
//
//	while (start < input_file_length)
//	{
//		instruction_from_file = (char*)malloc(100 * sizeof(char));
//		strcpy(instruction_from_file, instructions_table[start]);
//		start++;
//		char *token = (char*)malloc(100 * sizeof(char));
//		char *dummy_instruction = instruction_from_file;
//		token = strtok(instruction_from_file, " ");
//
//
//		if (strcmp(token, "READ") == 0)
//		{
//
//			token = strtok(NULL, "\n");
//			read(token);
//
//		}//End of READ
//
//		else if (strcmp(token, "MOV") == 0)
//		{
//			//token = strtok(NULL, " ");
//			char *t1 = (char*)malloc(2 * sizeof(char));
//			char *t2 = (char*)malloc(2 * sizeof(char));
//			t1 = strtok(NULL, ",");
//			t2 = strtok(NULL, "\n");
//			if (checkIfRegister(t2) == 1)
//			{
//				movRToM(t1, t2);//Left is destination, Right is source
//			}
//			else
//			{
//				movMToR(t1, t2);
//			}
//		}//End of MOV
//
//		else if (strcmp(token, "ADD") == 0)
//		{
//
//			//token = strtok(NULL, " ");
//			char *t1 = (char*)malloc(2 * sizeof(char));
//			char *t2 = (char*)malloc(2 * sizeof(char));
//			char *t3 = (char*)malloc(2 * sizeof(char));
//			t1 = strtok(NULL, ",");
//			t2 = strtok(NULL, ",");
//			t3 = strtok(NULL, "\n");
//			add(t1, t2, t3);
//
//		}//End of ADD
//	}
//}
//
//
//void printIntermediateTable()
//{
//	printf("InsNumber\tOpCode\tp1\tp2\tp3\tp4\tp5\n");
//	for (int i = 0; i < intermediate_table_pointer; i++)
//	{
//		printf("%d\t\t%d\t%d\t", intermediate_table[i]->instruction_number, intermediate_table[i]->opcode, intermediate_table[i]->p1);
//		if (intermediate_table[i]->p2 != -1)
//		{
//			printf("%d\t", intermediate_table[i]->p2);
//		}
//		else
//			printf("\t");
//		if (intermediate_table[i]->p3 != -1)
//		{
//			printf("%d\t", intermediate_table[i]->p3);
//		}
//		else
//			printf("\t");
//		if (intermediate_table[i]->p4 != -1)
//		{
//			printf("%d\t", intermediate_table[i]->p4);
//		}
//		else
//			printf("\t");
//		if (intermediate_table[i]->p5 != -1)
//		{
//			printf("%d\t", intermediate_table[i]->p5);
//		}
//		else
//			printf("\t\n\n");
//	}
//}
//
//void computeRead(int intermediate_table_index)
//{
//	printf("Enter the value to put in the register:");
//	scanf("%d", &registers[intermediate_table[intermediate_table_index]->p1]);
//}
//
//void computeAdd(int intermediate_table_index)
//{
//	registers[intermediate_table[intermediate_table_index]->p1] = registers[intermediate_table[intermediate_table_index]->p2] + registers[intermediate_table[intermediate_table_index]->p3];
//	printf("Sum is : %d", registers[intermediate_table[intermediate_table_index]->p1]);
//}
//
//
//void computeIntermediateInstructions()
//{
//	for (int i = 0; i < intermediate_table_pointer; i++)
//	{
//		if (intermediate_table[i]->opcode == 3)
//		{
//			computeAdd(i);
//		}
//		else if (intermediate_table[i]->opcode == 14)
//		{
//			computeRead(i);
//		}
//	}
//}
//

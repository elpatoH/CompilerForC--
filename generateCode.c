#include "generateCode.h"

void printQuad(Quad *quad);
void printASSG(Quad *quad);
void printPARAM(Quad *quad);
void printCALL(Quad *quad);
void printFUNDEF(Quad *quad);
void printLEAVE(Quad *quad);
void printRETURN(Quad *quad);
void printVAR(Quad *quad);

void generateCode(ASTnode *node)
{
	if (node == NULL)
		return;
	printQuad(node->code);
}

void printQuad(Quad *quad)
{
	switch (quad->op)
	{
	case FUNC_DEF:
	{
		printFUNDEF(quad);
		break;
	}
	case ASSG:
	{
		printASSG(quad);
		break;
	}
	case PARAM:
	{
		printPARAM(quad);
		break;
	}
	case CALL:
	{
		printCALL(quad);
		break;
	}
	case LEAVE:
	{
		printLEAVE(quad);
		break;
	}
	case RETURN:
	{
		printRETURN(quad);
		break;
	}
	case VAR:
	{
		printVAR(quad);
		break;
	}
	case EQ:
	case NE:
	case LE:
	case LT:
	case GE:
	case GT:{
		InfoNode* opr1Node = (InfoNode*) quad->src1;
		InfoNode* opr2Node = (InfoNode*) quad->src2;

		printf("\tlw $t3, ");
		//global no location
		if (strcmp(opr1Node->info, "global") == 0){
			printf("_%s\n", opr1Node->name);
		}

		//int variable in the negs init +4
		else if(strcmp(opr1Node->info, "") == 0){
			int* opr1Location = opr1Node->location;
			int opr1ActualLocation = 4*(*opr1Location) + 4;
			printf("-%d($fp)\n", opr1ActualLocation);
		}

		//param in the positives init +8
		else if (strcmp(opr1Node->info, "arg") == 0){
			int *argNum = opr1Node->argCount;
			int argLocation = *argNum * 4 + 8;
			printf("%d($fp) #useless?\n", argLocation);
		}

		//imm
		else if (strcmp(opr1Node->info, "temp") == 0){
			char *name = opr1Node->name;
			char *substring = &name[5];
			int number = atoi(substring);
			int location = (number * 4) + 4;
			printf("-%d($fp)\n", location);
		}

		/* - now for opr2 in $t4 - */

		printf("\tlw $t4, ");
		//global no location
		if (strcmp(opr2Node->info, "global") == 0){
			printf("_%s\n", opr2Node->name);
		}

		//int variable in the negs init +4
		else if(strcmp(opr2Node->info, "") == 0){
			int* opr2Location = opr2Node->location;
			int opr2ActualLocation = 4*(*opr2Location) + 4;
			printf("-%d($fp)\n", opr2ActualLocation);
		}

		//param in the positives init +8
		else if (strcmp(opr2Node->info, "arg") == 0){
			int *argNum = opr2Node->argCount;
			int argLocation = *argNum * 4 + 8;
			printf("%d($fp) #useless?\n", argLocation);
		}

		//imm
		else if (strcmp(opr2Node->info, "temp") == 0){
			char *name = opr2Node->name;
			char *substring = &name[5];
			int number = atoi(substring);
			int location = (number * 4) + 4;
			printf("-%d($fp)\n", location);
		}

		//now compare both
		switch (quad->op){
			case EQ: printf("\tseq $t5, $t3, $t4\n"); break;
			case NE: printf("\tsne $t5, $t3, $t4\n"); break;
			case LE: printf("\tsle $t5, $t3, $t4\n"); break;
			case LT: printf("\tslt $t5, $t3, $t4\n"); break;
			case GE: printf("\tsge $t5, $t3, $t4\n"); break;
			case GT: printf("\tsgt $t5, $t3, $t4\n"); break;
			default: fprintf(stderr, "not valid comparison\n"); exit(2);
		}
		
		Quad* dest = (Quad*) quad->dest;
		int* labelNumber = (int*) dest->src1;
		printf("\tbnez $t5, label_%d\n", *labelNumber);
		break;
	}
	case MUL:
	case ADD:
	case SUB:		
	case DIV: {
		InfoNode* opr1Node = (InfoNode*) quad->src1;
		InfoNode* opr2Node = (InfoNode*) quad->src2;

		/* - now for opr2 in $t5 - */

		printf("\tlw $t5, ");
		//global no location
		if (strcmp(opr1Node->info, "global") == 0){
			printf("_%s\n", opr1Node->name);
		}

		//int variable in the negs init +4
		else if(strcmp(opr1Node->info, "") == 0){
			int* opr1Location = opr1Node->location;
			int opr1ActualLocation = 4*(*opr1Location) + 4;
			printf("-%d($fp)\n", opr1ActualLocation);
		}

		//param in the positives init +8
		else if (strcmp(opr1Node->info, "arg") == 0){
			int *argNum = opr1Node->argCount;
			int argLocation = *argNum * 4 + 8;
			printf("%d($fp) #useless?\n", argLocation);
		}

		//imm
		else if (strcmp(opr1Node->info, "temp") == 0){
			char *name = opr1Node->name;
			char *substring = &name[5];
			int number = atoi(substring);
			int location = (number * 4) + 4;
			printf("-%d($fp)\n", location);
		}

		/* - now for opr2 in $t6 - */

		printf("\tlw $t6, ");
		//global no location
		if (strcmp(opr2Node->info, "global") == 0){
			printf("_%s\n", opr2Node->name);
		}

		//int variable in the negs init +4
		else if(strcmp(opr2Node->info, "") == 0){
			int* opr2Location = opr2Node->location;
			int opr2ActualLocation = 4*(*opr2Location) + 4;
			printf("-%d($fp)\n", opr2ActualLocation);
		}

		//param in the positives init +8
		else if (strcmp(opr2Node->info, "arg") == 0){
			int *argNum = opr2Node->argCount;
			int argLocation = *argNum * 4 + 8;
			printf("%d($fp) #useless?\n", argLocation);
		}

		//imm
		else if (strcmp(opr2Node->info, "temp") == 0){
			char *name = opr2Node->name;
			char *substring = &name[5];
			int number = atoi(substring);
			int location = (number * 4) + 4;
			printf("-%d($fp)\n", location);
		}

		/* - now operate on operands - */

		switch (quad->op){
			case MUL: printf("\tmulo $t7, $t5, $t6\n"); break;
			case ADD: printf("\tadd $t7, $t5, $t6\n"); break;
			case SUB: printf("\tsub $t7, $t5, $t6\n"); break;			
			case DIV: printf("\tdiv $t7, $t5, $t6\n"); break;
			default: fprintf(stderr, "not valid operator\n"); exit(2);
		}

		InfoNode* dest = (InfoNode*) quad->dest;
		char *name = dest->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int tempLocation = (number * 4) + 4;
		printf("	sw $t7, -%d($fp)\n\n", tempLocation);

		break;
	}
	case UMINUS:{
		InfoNode* opr1Node = (InfoNode*) quad->src1;

		/* - now for opr2 in $t5 - */

		printf("\tlw $t5, ");
		//global no location
		if (strcmp(opr1Node->info, "global") == 0){
			printf("_%s\n", opr1Node->name);
		}

		//int variable in the negs init +4
		else if(strcmp(opr1Node->info, "") == 0){
			int* opr1Location = opr1Node->location;
			int opr1ActualLocation = 4*(*opr1Location) + 4;
			printf("-%d($fp)\n", opr1ActualLocation);
		}

		//param in the positives init +8
		else if (strcmp(opr1Node->info, "arg") == 0){
			int *argNum = opr1Node->argCount;
			int argLocation = *argNum * 4 + 8;
			printf("%d($fp) #useless?\n", argLocation);
		}

		//imm
		else if (strcmp(opr1Node->info, "temp") == 0){
			char *name = opr1Node->name;
			char *substring = &name[5];
			int number = atoi(substring);
			int location = (number * 4) + 4;
			printf("-%d($fp)\n", location);
		}

		printf("\tsub $t7, $zero, $t5\n");

		InfoNode* dest = (InfoNode*) quad->dest;
		char *name = dest->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int tempLocation = (number * 4) + 4;
		printf("	sw $t7, -%d($fp)\n\n", tempLocation);

		break;
	}
	case RETRIEVE:{
		InfoNode* something = (InfoNode*)quad->dest;
		char *name = something->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int tempLocation = (number * 4) + 4;

		printf("\tadd $t0, $zero, $v0\n");
		printf("\tsw $t0 -%d($fp)\n", tempLocation);

		break;
	}
	case GOTO:{
		Quad* dest = (Quad*) quad->dest;
		int* labelNumber = (int*) dest->src1;
		printf("\tb label_%d\n", *labelNumber);
		break;
	}
	case LABEL:{
		int* labelNumber = (int*) quad->src1;
		printf("\nlabel_%d:\n", *labelNumber);
		break;
	}
	default:
	{
		fprintf(stderr, "not yet implemented: %s\n", operationName(quad->op));
		exit(2);
	}
	}

	if (quad->next)
	{
		printQuad(quad->next);
	}
}

void printVAR(Quad *quad)
{
	InfoNode *stRef = (InfoNode *)quad->dest;
	if (stRef == NULL)
		return;
	if (strcmp(stRef->info, "arg") == 0)
	{
		int *argNum = stRef->argCount;
		int argLocation = *argNum * 4 + 8;
		printf("    lw $t0, %d($fp) #useless?\n", argLocation);
	}
	else if (strcmp(stRef->type, "int variable") == 0)
	{
		// printf("wtf: %s\n", stRef->name);
		//  int* argNum = stRef->location;
		//  int argLocation = *argNum * 4 + 4;
		//  //if its not an arg it should be in neg stack space
		//  printf("    lw $t0, -%d($fp)\n", argLocation -4);
		//  printf("    sw $t1, -%d($fp)\n\n", argLocation);
	}
}

// end epilogue
void printRETURN(Quad *quad)
{
	InfoNode *node = quad->src1;
	if (strcmp(node->name, "main") == 0)
	{
		printf("li $v0, 10\n");
		printf("syscall\n");
	}
	else
	{
		printf("jr $ra\n\n");
	}
}

// start epilogue
void printLEAVE(Quad *quad)
{
	InfoNode *node = (InfoNode *)quad->src1;
	int *paramsC = node->argCount;
	int tempC = getTemporaryCount(symbolTable);
	int frame_size = 4 * ((*paramsC) + tempC) + 8;

	InfoNode* opr1Node = (InfoNode*) quad->dest;

	if (opr1Node != NULL){

	/* - now for opr2 in $t5 - */

	printf("\tlw $v0, ");
	//global no location
	if (strcmp(opr1Node->info, "global") == 0){
		printf("_%s\n", opr1Node->name);
	}

	//int variable in the negs init +4
	else if(strcmp(opr1Node->info, "") == 0){
		int* opr1Location = opr1Node->location;
		int opr1ActualLocation = 4*(*opr1Location) + 4;
		printf("-%d($fp)\n", opr1ActualLocation);
	}

	//param in the positives init +8
	else if (strcmp(opr1Node->info, "arg") == 0){
		int *argNum = opr1Node->argCount;
		int argLocation = *argNum * 4 + 8;
		printf("%d($fp) #useless?\n", argLocation);
	}

	//imm
	else if (strcmp(opr1Node->info, "temp") == 0){
		char *name = opr1Node->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int location = (number * 4) + 4;
		printf("-%d($fp)\n", location);
	}
	}

	printf("\n    move $sp, $fp\n");
	printf("    lw $ra, 0($sp)\n");
	printf("    lw $fp, 4($sp)\n");
	printf("    addiu $sp, $sp, %d\n", frame_size - 4);
}

void printFUNDEF(Quad *quad)
{
	InfoNode *node = (InfoNode *)quad->src1;
	int tempC = getTemporaryCount(symbolTable);
	int localC = getLocalCount(symbolTable);
	int frame_size = 4 * (tempC + localC);

	if (strcmp(node->name, "main") == 0){
		printf(".globl %s\n", node->name);
		printf("%s:\n", node->name);
	}
	else{
		printf(".globl _%s\n", node->name);
		printf("_%s:\n", node->name);
	}
	printf("    addiu $sp, $sp, -8     # 2 slots for fp and ra\n");
	printf("    sw $fp, 4($sp)         # Save the old frame pointer\n");
	printf("    sw $ra, 0($sp)         # Save the return address\n");
	printf("    move $fp, $sp          # Set the new frame pointer\n");
	if (frame_size != 0)
	{
		printf("    addiu $sp, $sp, -%d    # tmps: %d, locals: %d\n", frame_size, tempC, localC);
	}
	printf("\n");
}

void printCALL(Quad *quad)
{
	InfoNode *stRef = (InfoNode *)quad->src1;
	printf("    jal _%s\n", stRef->name);
	int *paramC = stRef->argCount;
	int restoreSpace = *paramC * 4;
	printf("    addiu $sp, $sp, %d\n\n", restoreSpace);
}

void printPARAM(Quad *quad)
{
	InfoNode *opr1Node = (InfoNode *)quad->src1;

	printf("\tlw $t0, ");
	//global no location
	if (strcmp(opr1Node->info, "global") == 0){
		printf("_%s\n", opr1Node->name);
	}

	//int variable in the negs init +4
	else if(strcmp(opr1Node->info, "") == 0){
		int* opr1Location = opr1Node->location;
		int opr1ActualLocation = 4*(*opr1Location) + 4;
		printf("-%d($fp)\n", opr1ActualLocation);
	}

	//param in the positives init +8
	else if (strcmp(opr1Node->info, "arg") == 0){
		int *argNum = opr1Node->argCount;
		int argLocation = *argNum * 4 + 8;
		printf("%d($fp) #useless?\n", argLocation);
	}

	//imm
	else if (strcmp(opr1Node->info, "temp") == 0){
		char *name = opr1Node->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int location = (number * 4) + 4;
		printf("-%d($fp)\n", location);
	}
	// get place in stack from symboltable index or something like that
	printf("    addiu $sp, $sp, -4\n");
	printf("    sw $t0, 0($sp)\n\n");
}

void printASSG(Quad *quad)
{
	int *myNoOp = (int *)quad->src2;
	if (myNoOp != NULL)
	{
		//src1 is right hand side, 
		//dest is left hand side
		InfoNode *node = (InfoNode *)quad->src1;
		char *name = node->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int location;
		if (strcmp(node->info, "arg") == 0)
		{
			int *argNum = node->argCount;
			location = *argNum * 4 + 8;
		}
		else
		{
			location = (number * 4) + 4;
		}

		InfoNode *stREF = (InfoNode *)quad->dest;
		int LHSLocation = ((*stREF->location) * 4) + 4;

		if (strcmp(node->info, "arg") == 0)
		{
			printf("    lw $t1, %d($fp)\n", location);
		}
		else
		{
			if (strcmp(node->info, "global") == 0)
			{
				printf("    la $t1, _%s\n", name);
				printf("    lw $t1, 0($t1)\n");
			}
			else if (strcmp(node->info, "") == 0){
				int* rhsLocation = node->location;
				int rhsActualLocation = 4*(*rhsLocation) + 4;
				printf("    lw $t1, -%d($fp) # popo\n", rhsActualLocation);
			}
			else
			{
				printf("    lw $t1, -%d($fp) # name: %s\n", location, name);
			}
		}

		if (strcmp(stREF->info, "global") == 0)
		{
			printf("    sw $t1, _%s  # : _%s :\n\n", stREF->name, stREF->name);
		}
		else
		{
			if (strcmp(stREF->info, "arg") == 0)
			{
				int *argLoc = stREF->argCount;
				int LHSArgLocation = *argLoc * 4 + 8;
				printf("    sw $t1, %d($fp)  # : _%s :\n\n", LHSArgLocation, stREF->name);
			}
			else if (strcmp(stREF->info, "") == 0){
				int* lhsLocation = stREF->location;
				int lhsActualLocation = 4*(*lhsLocation) + 4;
				printf("    sw $t1, -%d($fp) # popo\n", lhsActualLocation);
			}
			else
			{
				printf("    sw $t1, -%d($fp)  # : _%s :\n\n", LHSLocation, stREF->name);
			}
		}
	}
	else
	{
		InfoNode *node = (InfoNode *)quad->dest;
		char *name = node->name;
		char *substring = &name[5];
		int number = atoi(substring);
		int location = (number * 4) + 4;
		
		int *src1 = (int *)quad->src1;
		printf("    li $t0, %d\n", *src1);
		printf("    sw $t0, -%d($fp)\n\n", location);
	}
}

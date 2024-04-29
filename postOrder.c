#include "postOrder.h"

//global for return case
char* current_function_name ;

void codeGen_bool(ASTnode* e, Quad* trueDst, Quad* falseDst);

void generateGlobalVariables()
{
	ScopeNode *globalSymbolTable = getLastScope(symbolTable);
	if (globalSymbolTable == NULL)
		return;

	bool first = true;
	// iterate through global table finding all global variables
	InfoNode *variable = globalSymbolTable->variableList;
	while (variable != NULL)
	{
		if (strcmp(variable->type, "int variable") == 0 && strcmp(variable->info, "global") == 0)
		{
			if (first)
			{
				first = false;
				printf(".data\n");
			}
			char *name = variable->name;
			printf("_%s : .word 0\n", name);
		}
		variable = variable->next;
	}
	printf("\n.text\n\n");
	return;
}

void postOrderTraversal(ASTnode *node)
{
	if (node == NULL)
		return;

	name_num = 0;

	visit(node);
}

void visit(ASTnode *node)
{
	codeGen_expr(node);
}

void codeGen_expr(ASTnode *e)
{
	switch (e->ntype)
	{
	case FUNC_DEF:
	{
		// char* name = func_def_name(e);
		// get args and do something with them
		ASTnode *body = func_def_body(e);
		char *name = func_def_name(e);
		current_function_name = name;
		if (body != NULL)
		{
			codeGen_expr(body);
		}

		ScopeNode *globalScope = getLastScope(symbolTable);
		InfoNode *stREF = findFunctionInScopeAndGetArgCount(globalScope, name);

		Quad *funcDefInst = newinstr(FUNC_DEF, stREF, e->num, NULL);
		if (body != NULL)
		{
			funcDefInst->next = body->code;
		}
		else
		{
			funcDefInst->next = NULL;
		}

		e->code = funcDefInst;
		// setting leave and return 3addr
		Quad *temp = e->code;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		Quad *leaveQuad = newinstr(LEAVE, stREF, NULL, NULL);
		Quad *returnQuad = newinstr(RETURN, stREF, NULL, NULL);
		temp->next = leaveQuad;
		leaveQuad->next = returnQuad;
		break;
	}
	case STMT_LIST:
	{
		// if the list_hd is an if we might need to do something crazy
		ASTnode *list_hd = (ASTnode *)stmt_list_head(e);
		codeGen_expr(list_hd);
		e->code = list_hd->code;

		// point to last
		Quad *lastPointer = e->code;
		while (lastPointer->next != NULL)
		{
			lastPointer = lastPointer->next;
		}

		ASTnode *list_rest = (ASTnode *)stmt_list_rest(e);
		if (list_rest != NULL)
		{
			codeGen_expr(list_rest);
			lastPointer->next = list_rest->code;
		}
		break;
	}
	case IF:
	{
		ASTnode* if_expr = (ASTnode*) stmt_if_expr(e);
		ASTnode* if_then = (ASTnode*) stmt_if_then(e);
		ASTnode* if_else = (ASTnode*) stmt_if_else(e);
		Quad* label_then = newlabel();
		Quad* label_else = newlabel();
		Quad* label_after = newlabel();
		
		codeGen_bool(if_expr, label_then, label_else);
		if (if_then != NULL){
			codeGen_expr(if_then);
		}
		if (if_else != NULL){
			codeGen_expr(if_else);
		}
		e->code = if_expr->code;

		// point to last
		Quad* lastPointer = e->code;
		while (lastPointer->next != NULL)
		{
			lastPointer = lastPointer->next;
		}

		//append label then
		lastPointer->next = label_then;
		while (lastPointer->next != NULL)
		{
			lastPointer = lastPointer->next;
		}

		if (if_then != NULL){
			lastPointer->next = if_then->code;
			while (lastPointer->next != NULL)
			{
				lastPointer = lastPointer->next;
			}
		}

		lastPointer->next = newinstr(GOTO, NULL, NULL, label_after);
		while (lastPointer->next != NULL)
		{
			lastPointer = lastPointer->next;
		}

		lastPointer->next = label_else;
		while (lastPointer->next != NULL)
		{
			lastPointer = lastPointer->next;
		}

		if (if_else != NULL){
			lastPointer->next = if_else->code;
			while (lastPointer->next != NULL)
			{
				lastPointer = lastPointer->next;
			}
		}

		lastPointer->next = label_after;

		break;
	}
	case WHILE: 
	{
		Quad* label_top = newlabel();
		Quad* label_body = newlabel();
		Quad* label_after = newlabel();

		ASTnode* expr = stmt_while_expr(e); // B
		ASTnode* body = stmt_while_body(e); // S

		codeGen_bool(expr, label_body, label_after);
		if (body != NULL){
			codeGen_expr(body);
		}

		// Ltop
		e->code = label_top;

		Quad* temp = e->code;
		while(temp->next != NULL){
			temp = temp->next;
		}

		//B.code
		temp->next = expr->code;

		temp = e->code;
		while(temp->next != NULL){
			temp = temp->next;
		}

		//Lbody
		temp->next = label_body;

		temp = e->code;
		while(temp->next != NULL){
			temp = temp->next;
		}

		//S1.code
		if (body != NULL){
			temp->next = body->code;

			temp = e->code;
			while(temp->next != NULL){
				temp = temp->next;
			}
		}

		//newinst
		temp->next = newinstr(GOTO, NULL, NULL, label_top);

		temp = e->code;
		while(temp->next != NULL){
			temp = temp->next;
		}

		//Lafter
		temp->next = label_after;

		break;
	}
	case ASSG:
	{

		char *LHSName = stmt_assg_lhs(e);
		InfoNode *LHSSTRef = findVariableInAllScopes(symbolTable, LHSName);

		ASTnode *node = stmt_assg_rhs(e);
		codeGen_expr(node);
		e->code = node->code;

		int *loc = malloc(sizeof(int));
		*loc = name_num;
		LHSSTRef->location = loc;
		if (strcmp(LHSSTRef->info, "global") != 0)
		{
			name_num++;
		}

		int *noOpMine = malloc(sizeof(int));
		*noOpMine = 1;

		// REDO
		// when we have operators the RHS is stored in node.place NOT node.code.dest
		Quad *perro = newinstr(ASSG, node->place, noOpMine, LHSSTRef);

		Quad* temp = e->code;
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = perro;

		// save location where it is being saved
		//  and add new inst for assignemnt to
		break;
	}
	case RETURN:
	{
		ScopeNode *globalScope = getLastScope(symbolTable);
		InfoNode *stREF = findFunctionInScopeAndGetArgCount(globalScope, current_function_name);

		Quad *leaveQuad = newinstr(LEAVE, stREF, NULL, NULL);
		Quad *returnQuad = newinstr(RETURN, stREF, NULL, NULL);

		e->code = leaveQuad;
		e->code->next = returnQuad;
		break;
	}
	case FUNC_CALL:
	{
		ASTnode *arglist = (ASTnode *)func_call_args(e);
		codeGen_expr(arglist);

		// if statement to check if return type of func_call is void
		// if void then no need to set e.place

		// reverse list so params are in right order
		Quad *reversedCodeList = NULL;
		Quad *current = arglist->code;
		while (current != NULL)
		{
			Quad *next = current->next;
			current->next = reversedCodeList;
			reversedCodeList = current;
			current = next;
		}

		e->code = reversedCodeList;

		// need loop to iterate through num of args
		ScopeNode *globalScope = getLastScope(symbolTable);
		InfoNode *stREF = findFunctionInScopeAndGetArgCount(globalScope, e->nameF);

		Quad *list = e->code;
		int index = 0;
		while (index < *stREF->argCount)
		{
			Quad *newParam = newinstr(PARAM, list->dest, NULL, NULL);
			newParam->next = list->next;
			list->next = newParam;

			list = list->next->next;
			index++;
		}

		// last instruction is CALL
		Quad *temp = e->code;
		if (temp == NULL)
		{
			e->code = newinstr(CALL, stREF, NULL, NULL);
			break;
		}
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		temp->next = newinstr(CALL, stREF, NULL, NULL);

		// retrieve function
		break;
	}
	case EXPR_LIST:
	{
		ASTnode *list_hd = (ASTnode *)expr_list_head(e);

		if (list_hd != NULL)
		{
			codeGen_expr(list_hd);
			e->code = list_hd->code;
			e->place = list_hd->place;

			// Point to the last element in the generated code list
			Quad *lastPointer = e->code;
			while (lastPointer->next != NULL)
			{
				lastPointer = lastPointer->next;
			}

			ASTnode *list_rest = (ASTnode *)expr_list_rest(e);
			if (list_rest != NULL)
			{
				codeGen_expr(list_rest);
				lastPointer->next = list_rest->code;
			}
		}
		break;
	}
	case INTCONST:
	{
		e->place = newtemp("INTCON");
		int *num = e->num;
		e->code = newinstr(ASSG, num, NULL, e->place);
		break;
	}
	case IDENTIFIER:
	{
		InfoNode *stREF = findVariableInAllScopes(symbolTable, e->nameF);
		if (stREF == NULL)
			return;
		e->place = stREF;
		e->code = newinstr(VAR, NULL, NULL, stREF);
		break;
	}
	case UMINUS:
	{
		ASTnode* operand1 = (ASTnode*) expr_operand_1(e);
		codeGen_expr(operand1);

		e->place = newtemp("INTCON");
		e->code = operand1->code;

		e->code->next = newinstr(UMINUS, operand1, NULL, e->place);
		break;
	}
	case MUL:
	case DIV:
	case SUB:
	case ADD:
	{
		ASTnode* operand1 = (ASTnode*) expr_operand_1(e);
		ASTnode* operand2 = (ASTnode*) expr_operand_2(e);
		codeGen_expr(operand1);
		codeGen_expr(operand2);

		e->place = newtemp("INTCON");
		e->code = operand1->code;

		e->code->next = operand2->code;
		e->code->next->next = newinstr(e->ntype, operand1, operand2, e->place);
		break;
	}
	default:
	{
		char *perro = operationName(e->ntype);
		fprintf(stderr, "*** Unrecognized statement: %s\n", perro);
		exit(1);
	}
	}
}

void codeGen_bool(ASTnode* e, Quad* trueDst, Quad* falseDst){
	ASTnode* opr1 = expr_operand_1(e);
	ASTnode* opr2 = expr_operand_2(e);
	codeGen_expr(opr1);
	codeGen_expr(opr2);

	//may want to change truedst to just strings for easier access
	e->code = opr1->code;
	e->code->next = opr2->code;
	Quad* if_instr_true = newinstr(e->ntype, opr1, opr2, trueDst);
	e->code->next->next = if_instr_true;
	Quad* if_instr_goto = newinstr(GOTO, NULL, NULL, falseDst);
	e->code->next->next->next = if_instr_goto;
}

InfoNode *newtemp(char *type)
{
	char nameBuffer[50];
	snprintf(nameBuffer, sizeof(nameBuffer), "Temp_%d", name_num++);
	InfoNode *newTemporary = createVariableNode(nameBuffer, type, "temp", NULL);

	addVariablePointerToScope(&symbolTable, newTemporary);
	return newTemporary;
}

Quad *newinstr(NodeType op, void *src1, void *src2, void *dest)
{
	Quad *ninstr = malloc(sizeof(Quad));
	ninstr->op = op;
	ninstr->src1 = src1;
	ninstr->src2 = src2;
	ninstr->dest = dest;
	ninstr->next = NULL;

	return ninstr;
}

Quad *newlabel()
{
	int *cur_label = malloc(sizeof(int));
	*cur_label = label_num;
	label_num++;
	return newinstr(LABEL, cur_label, NULL, NULL);
}


char *operationName(NodeType ntype)
{
	switch (ntype)
	{
	case EQ:
		return "==";
	case NE:
		return "!=";
	case LE:
		return "<=";
	case LT:
		return "<";
	case GE:
		return ">=";
	case GT:
		return ">";
	case ADD:
		return "+";
	case SUB: /* fall through */
	case UMINUS:
		return "-";
	case MUL:
		return "*";
	case DIV:
		return "/";
	case AND:
		return "&&";
	case OR:
		return "||";
	case INTCONST:
		return "intconst";
	case EXPR_LIST:
		return "expr_list";
	case FUNC_DEF:
		return "func_def";
	case STMT_LIST:
		return "stmt_list";
	case LABEL:
		return "label";
	case PARAM:
		return "param";
	case CALL:
		return "call";
	case ASSG:
		return "assignment";
	case FUNC_CALL:
		return "func_call";
	case IDENTIFIER:
		return "identifier";
	case VAR:
		return "var";
	case IF:
		return "if";
	case GOTO:
		return "goto";

	default:
		fprintf(stderr, "Unrecognized syntax tree node type %d\n", ntype);
		return NULL;
	}
}
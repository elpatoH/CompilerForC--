#include "postOrder.h"

char *operationName(NodeType ntype);

void printQuad(Quad* quad);
void printASSG(Quad* quad);
void printPARAM(Quad* quad);
void printCALL(Quad* quad);
void printFUNDEF(Quad* quad);
void printLEAVE(Quad* quad);
void printRETURN(Quad* quad);

void postOrderTraversal(ASTnode* node) {
    if (node == NULL) return;

    postOrderTraversal(node->child0);
    postOrderTraversal(node->child1);
    postOrderTraversal(node->child2);
    
    visit(node);
}

void visit(ASTnode* node) {
    codeGen_expr(node);
}

Quad* concatQuads(Quad* list1, Quad* list2) {
    if (list1 == NULL)
        return list2;
    Quad* tmp = list1;
    while (tmp->next != NULL) {
        tmp = tmp->next;
    }
    tmp->next = list2;
    return list1;
}

void codeGen_expr(ASTnode* e){
    switch (e->ntype) {
        case FUNC_DEF: {
            //char* name = func_def_name(e);
            //get args and do something with them
            ASTnode* body = func_def_body(e);
            codeGen_expr(body);

            char* name = func_def_name(e);
            ScopeNode* globalScope = getLastScope(symbolTable);
            InfoNode* stREF = findFunctionInScopeAndGetArgCount(globalScope, name);
            
            Quad* funcDefInst = newinstr(FUNC_DEF, stREF, NULL, NULL);
            funcDefInst->next = body->code;
            e->code = funcDefInst;

            //setting leave and return 3addr
            Quad* temp = e->code;
            while(temp->next != NULL){
                temp = temp->next;
            }
            Quad* leaveQuad = newinstr(LEAVE, stREF, NULL, NULL);
            Quad* returnQuad = newinstr(RETURN, NULL, NULL, NULL);
            temp->next = leaveQuad;
            leaveQuad->next = returnQuad;
            break;
        }
        case STMT_LIST: {
            ASTnode* list_hd = (ASTnode*) stmt_list_head(e);
            codeGen_expr(list_hd);
            e->code = list_hd->code;

            Quad* lastPointer = e->code;
            while (lastPointer->next != NULL){
                lastPointer = lastPointer->next;
            }
            ASTnode* list_rest = (ASTnode*) stmt_list_rest(e);
            if (list_rest != NULL){
                codeGen_expr(list_rest);
                lastPointer->next = list_rest->code;
            }
            break;
        }
        case FUNC_CALL: {
            ASTnode* arglist = (ASTnode*) func_call_args(e);
            //recurse into argument list expression
            codeGen_expr(arglist); 
            //if statement to check if return type of func_call is void
            //if void then no need to set e.place
            e->code = arglist->code;
            //loop to iterate through num of args
            e->code->next = newinstr(PARAM, e->code->dest, NULL, NULL);

            //iterate through symbol table to get name of callee
            ScopeNode* globalScope = getLastScope(symbolTable);
            InfoNode* stREF = findFunctionInScopeAndGetArgCount(globalScope, e->nameF);
            e->code->next->next = newinstr(CALL, stREF, NULL, NULL);
            //retrieve function
            break;
        }
        case EXPR_LIST: {
            //printf("im in exprlist codegenexpr\n child0 contains head and child1 contains the rest\n");
            fflush(stdout);
            //everything commented out here is will eventually handle multiple parameters
            ASTnode* list_hd;
            //ASTnode* list_tl;
            list_hd = (ASTnode*) expr_list_head(e);
            //list_tl = (ASTnode*) expr_list_rest(e);
            if (list_hd != NULL){
                codeGen_expr(list_hd);
                e->code = list_hd->code;
                e->place = list_hd->place;
                // if (list_tl != NULL){
                //     codeGen_expr(list_tl);
                //     e->code->next = list_tl->code;
                // }
            }
            break;
        }
        case INTCONST: {
            e->place = newtemp("INTCON");
            int* num = e->num;
            e->code = newinstr(ASSG, num, NULL, e->place);
            break;
        }
        default:{
            char* perro = operationName(e->ntype);
            fprintf(stderr, "*** Unrecognized statement: %s\n", perro);
            return;
        }
    }
}

InfoNode* newtemp(char* type)
{
    char nameBuffer[50];
    snprintf(nameBuffer, sizeof(nameBuffer), "Temp_%d", name_num++);
    InfoNode* newTemporary = createVariableNode(nameBuffer, type, "temp", NULL);

    addVariablePointerToScope(&symbolTable, newTemporary);
    return newTemporary;
}

Quad* newinstr(NodeType op, void* src1, void* src2, void* dest) {
    Quad* ninstr = malloc(sizeof(Quad));
    ninstr->op = op;
    ninstr->src1 = src1;
    ninstr->src2 = src2;
    ninstr->dest = dest;
    ninstr->next = NULL;

    return ninstr;
}

Quad* newlabel()
{
    label_num++;
    return newinstr(LABEL, &label_num, NULL, NULL);
}

void generateCode(ASTnode* node){
    if (node == NULL) return;
    printQuad(node->code);
}

void printQuad(Quad* quad) {
    switch (quad->op){
        case FUNC_DEF: {
            printFUNDEF(quad);
            break;
        }
        case ASSG: {
            printASSG(quad);
            break;
        }
        case PARAM: {
            printPARAM(quad);
            break;
        }
        case CALL: {
            printCALL(quad);
            break;
        }
        case LEAVE: {
            printLEAVE(quad);
            break;
        }
        case RETURN: {
            printRETURN(quad);
            break;
        }
        default: {
            printf("not yet implemented: %s\n", operationName(quad->op));
            exit(0);
        }
    }
    
    if (quad->next) {
        printQuad(quad->next);
    }
}

//end epilogue
void printRETURN(Quad* quad){
    printf("jr $ra\n\n");
}

//start epilogue
void printLEAVE(Quad* quad){
    InfoNode* node = (InfoNode*) quad->src1;
    //printf("\n    leave: %s\n", node->name);
    //need the node so that i can see how many vars im gonna need and how much i would need to deallocate
    printf("\n    lw    $ra, 4($sp)\n");
    printf("    lw    $fp, 0($sp)\n");
    printf("    addiu $sp, $sp, 24\n");
}

void printFUNDEF(Quad* quad){
    InfoNode* node = (InfoNode*) quad->src1;
    printf(".globl %s\n", node->name);
    printf("%s:\n", node->name);
    printf("    addiu $sp, $sp, -24\n");
    printf("    sw $fp, 0($sp)\n");
    printf("    sw    $ra, 4($sp)\n");
    printf("    addiu $fp, $sp, 20\n\n");

}

void printCALL(Quad* quad){
    InfoNode* stRef = (InfoNode*) quad->src1;
    printf("    jal %s\n", stRef->name);
}

void printPARAM(Quad* quad){
    InfoNode* node = (InfoNode*) quad->src1;
    //get place in stack from symboltable index or something like that
    printf("    add $a0, $zero, $t0\n");
}

void printASSG(Quad* quad){
    InfoNode* node = (InfoNode*) quad->dest;
    int* src1 = (int*) quad->src1;
    printf("    li $t0, %d\n", *src1);
}

char* operationName(NodeType ntype) {
  switch (ntype) {
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
  case SUB:        /* fall through */
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


    
  default:
      fprintf(stderr, "*** [%s] Unrecognized syntax tree node type %d\n", __func__, ntype);
      return NULL;
  }
}

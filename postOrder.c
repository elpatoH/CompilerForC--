#include "postOrder.h"

char *operationName(NodeType ntype);

void printQuad(Quad* quad);
void printASSG(Quad* quad);
void printPARAM(Quad* quad);
void printCALL(Quad* quad);
void printFUNDEF(Quad* quad);
void printLEAVE(Quad* quad);
void printRETURN(Quad* quad);
void printVAR(Quad* quad);

void generateGlobalVariables(){
  ScopeNode* globalSymbolTable = getLastScope(symbolTable);
  if (globalSymbolTable == NULL) return;

  bool first = true;
  //iterate through global table finding all global variables 
  InfoNode* variable = globalSymbolTable->variableList;
  while (variable != NULL) {
      if (strcmp(variable->type, "int variable") == 0 && strcmp(variable->info, "global") == 0) {
          if (first){
            first = false;
            printf(".data\n");
          }
          char* name = variable->name;
          printf("%s : .word 0\n", name);
      }
      variable = variable->next;
  }
  printf("\n.text\n\n");
  return;
}

void postOrderTraversal(ASTnode* node) {
    if (node == NULL) return;

    // postOrderTraversal(node->child0);
    // postOrderTraversal(node->child1);
    // postOrderTraversal(node->child2);

    name_num = 0;
    
    visit(node);
}

void visit(ASTnode* node) {
    codeGen_expr(node);
}

void codeGen_expr(ASTnode* e){
    // if (e->nameF != NULL){
    //   char* popo = operationName(e->ntype);
    //   printf("HERERERER %s : %s\n\n\n", popo, e->nameF);
    // }
    switch (e->ntype) {
        case FUNC_DEF: {
            //char* name = func_def_name(e);
            //get args and do something with them
            ASTnode* body = func_def_body(e);
            if (body != NULL){
                codeGen_expr(body);
            }

            char* name = func_def_name(e);
            ScopeNode* globalScope = getLastScope(symbolTable);
            InfoNode* stREF = findFunctionInScopeAndGetArgCount(globalScope, name);

            Quad* funcDefInst = newinstr(FUNC_DEF, stREF,e->num, NULL);
            if (body != NULL){
                funcDefInst->next = body->code;
            }
            else{
                funcDefInst->next = NULL;
            }

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

            //point to last
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
        case ASSG: {

          char* LHSName = stmt_assg_lhs(e);
          InfoNode* LHSSTRef = findVariableInAllScopes(symbolTable, LHSName);
          
          ASTnode* node = stmt_assg_rhs(e);
          codeGen_expr(node);
          e->code = node->code;

          int* loc = malloc(sizeof(int));
          *loc = name_num;
          LHSSTRef->location = loc;

          name_num++;

          int* noOpMine = malloc(sizeof(int));
          *noOpMine = 1;

          Quad* perro = newinstr(ASSG, node->code->dest, noOpMine, LHSSTRef);
          e->code->next = perro;

          //save location where it is being saved
          // and add new inst for assignemnt to
          break;
        }
        case FUNC_CALL: {
            ASTnode* arglist = (ASTnode*) func_call_args(e);
            codeGen_expr(arglist); 

            //if statement to check if return type of func_call is void
            //if void then no need to set e.place

            //reverse list so params are in right order
            Quad* reversedCodeList = NULL;
            Quad* current = arglist->code;
            while (current != NULL) {
                Quad* next = current->next;
                current->next = reversedCodeList;
                reversedCodeList = current;
                current = next;
            }

            e->code = reversedCodeList;

            //need loop to iterate through num of args
            ScopeNode* globalScope = getLastScope(symbolTable);
            InfoNode* stREF = findFunctionInScopeAndGetArgCount(globalScope, e->nameF);

            Quad* list = e->code;
            int index = 0;
            while (index < *stREF->argCount){
                Quad* newParam = newinstr(PARAM, list->dest, NULL, NULL);
                newParam->next = list->next;
                list->next = newParam;


                list = list->next->next;
                index++;
            }

            //last instruction is CALL
            Quad* temp = e->code;
            while(temp->next != NULL){
                temp = temp->next;
            }
            temp->next = newinstr(CALL, stREF, NULL, NULL);

            //retrieve function
            break;
        }
        case EXPR_LIST: {
            ASTnode* list_hd = (ASTnode*) expr_list_head(e);
            
            if (list_hd != NULL){
                codeGen_expr(list_hd);
                e->code = list_hd->code;
                e->place = list_hd->place;

                // Point to the last element in the generated code list
                Quad* lastPointer = e->code;
                while (lastPointer->next != NULL) {
                    lastPointer = lastPointer->next;
                }

                ASTnode* list_rest = (ASTnode*) expr_list_rest(e);
                if (list_rest != NULL){
                    codeGen_expr(list_rest);
                    lastPointer->next = list_rest->code;
                }
            }
            break;
        }
        case INTCONST: {
            e->place = newtemp("INTCON");
            int* num = e->num;
            e->code = newinstr(ASSG, num, NULL, e->place);
            break;
        }
        case IDENTIFIER: {
            InfoNode* stREF = findVariableInAllScopes(symbolTable, e->nameF);
            if (stREF == NULL) printf("wtf\n");
            fflush(stdout);
            e->code = newinstr(VAR, NULL, NULL, stREF);
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
        case VAR: {
            printVAR(quad);
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

void printVAR(Quad* quad){
    InfoNode* stRef = (InfoNode*) quad->dest;
    if (stRef == NULL) return;
    if (strcmp(stRef->info, "arg") == 0){
      int* argNum = stRef->argCount;
      int argLocation = *argNum * 4 + 8;
      printf("    lw $t0, %d($fp) #useless?\n", argLocation);
    }
    else if (strcmp(stRef->type, "int variable") == 0){
      //printf("wtf: %s\n", stRef->name);
      // int* argNum = stRef->location;
      // int argLocation = *argNum * 4 + 4;
      // //if its not an arg it should be in neg stack space
      // printf("    lw $t0, -%d($fp)\n", argLocation -4);
      // printf("    sw $t1, -%d($fp)\n\n", argLocation);
    }
}

//end epilogue
void printRETURN(Quad* quad){
    printf("jr $ra\n\n");
}

//start epilogue
void printLEAVE(Quad* quad){
    InfoNode* node = (InfoNode*) quad->src1;
    int* paramsC = node->argCount;
    int tempC = getTemporaryCount(symbolTable);
    int frame_size = 4 * ((*paramsC) + tempC) + 8;

    printf("\n    move $sp, $fp\n");
    printf("    lw $ra, 0($sp)\n");
    printf("    lw $fp, 4($sp)\n");
    printf("    addiu $sp, $sp, %d\n", frame_size);
}

void printFUNDEF(Quad* quad){
    InfoNode* node = (InfoNode*) quad->src1;
    int tempC = getTemporaryCount(symbolTable);
    int localC = getLocalCount(symbolTable);
    int frame_size = 4 * (tempC + localC);

    printf(".globl %s\n", node->name);
    printf("%s:\n", node->name);
    printf("    addiu $sp, $sp, -8     # 2 slots for fp and ra\n");
    printf("    sw $fp, 4($sp)         # Save the old frame pointer\n");
    printf("    sw $ra, 0($sp)         # Save the return address\n");
    printf("    move $fp, $sp          # Set the new frame pointer\n");
    if (frame_size != 0){
      printf("    addiu $sp, $sp, -%d    # tmps: %d, locals: %d\n", frame_size, tempC, localC);
    }
    printf("\n");
}

void printCALL(Quad* quad){
    InfoNode* stRef = (InfoNode*) quad->src1;
    printf("    jal %s\n", stRef->name);
    int* paramC = stRef->argCount;
    int restoreSpace = *paramC*4;
    printf("    addiu $sp, $sp %d\n\n", restoreSpace);
}

void printPARAM(Quad* quad){
    InfoNode* node = (InfoNode*) quad->src1;
    if (node == NULL) return;
    if (strcmp(node->info, "temp") == 0){
      char* name = node->name;
      char* substring = &name[5];
      int number = atoi(substring);
      int location = (number * 4) + 4;
      printf("    lw $t0, -%d($fp)\n", location);
    }
    else if (strcmp(node->type, "int variable") == 0 && strcmp(node->info, "global") == 0){
      printf("    lw $t0, %s\n", node->name);
    }
    //its an int variable and its not and argument
    else if (strcmp(node->type, "int variable") == 0 && strcmp(node->info, "arg") != 0){
      InfoNode* stREF = findVariableInScopeAndGetArgCount(symbolTable, node->name);
      int varLocation = ((*stREF->location) * 4) + 4;
      printf("    lw $t0, -%d($fp)\n", varLocation);
    }
    //get place in stack from symboltable index or something like that
    printf("    addiu $sp, $sp, -4\n");
    printf("    sw $t0, 0($sp)\n\n");
}

void printASSG(Quad* quad){
    int* myNoOp = (int*) quad->src2;
    if (myNoOp != NULL){
        InfoNode* node = (InfoNode*) quad->src1;
        char* name = node->name;
        char* substring = &name[5];
        int number = atoi(substring);
        int location;
        if (strcmp(node->info, "arg") == 0){
          int* argNum = node->argCount;
          location =  *argNum * 4 + 8;
        }
        else{
          location = (number * 4) + 4;
        }

        InfoNode* stREF = (InfoNode*) quad->dest;
        int LHSLocation = ((*stREF->location) * 4) + 4;

        if (strcmp(node->info, "arg") == 0){
          printf("    lw $t1, %d($fp)\n", location);
        }
        else{
          printf("    lw $t1, -%d($fp)\n", location);
        }

        if (strcmp(stREF->info, "global") == 0){
          printf("    sw $t1, %s  # : %s :\n\n", stREF->name, stREF->name);
        }
        else{
          printf("    sw $t1, -%d($fp)  # : %s :\n\n", LHSLocation, stREF->name);
        }
    }
    else{
        InfoNode* node = (InfoNode*) quad->dest;
        char* name = node->name;
        char* substring = &name[5];
        int number = atoi(substring);
        int location = (number * 4) + 4;

        int* src1 = (int*) quad->src1;
        printf("    li $t0, %d\n", *src1);
        printf("    sw $t0, -%d($fp)\n\n", location);
    }
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
  case IDENTIFIER:
    return "identifier";
  case VAR:
    return "var";


    
  default:
      fprintf(stderr, "Unrecognized syntax tree node type %d\n", ntype);
      return NULL;
  }
}

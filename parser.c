/*
name: Daniel Gil
class: cs453
description: syntax analyzer for c--

REFORMAT comment is for changing function names to something more fitting
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "symbolTable.h"
#include "postOrder.h"
#include "ast.h"
#include "generateCode.h"

extern int get_token();
extern int line_num;
extern char* lexeme;
extern int chk_decl_flag;
extern int gen_code_flag;
Token curr_tok;

int currentScope = 0;
ScopeNode* symbolTable = NULL;
char* currentID;
int tmpCount = 0;

extern int print_ast_flag;


char* token_name[] = {
  "UNDEF",
  "ID",
  "INTCON",
  "LPAREN",
  "RPAREN",
  "LBRACE",
  "RBRACE",
  "COMMA",
  "SEMI",
  "kwINT",
  "kwIF",
  "kwELSE",
  "kwWHILE",
  "kwRETURN",
  "opASSG",
  "opADD",
  "opSUB",
  "opMUL",
  "opDIV",
  "opEQ",
  "opNE",
  "opGT",
  "opGE",
  "opLT",
  "opLE",
  "opAND",
  "opOR",
  "opNOT",
};

//function prototypes
void match(Token expected);
void prog();
ASTnode* func_defn();
void type();
void opt_formals(int* argCount);
void opt_var_decls();
ASTnode* opt_stmt_list();
ASTnode* stmt();
ASTnode* fn_call();
ASTnode* opt_expr_list(int* argumentCount);
void formals(int* argCount);
void var_decl();
void id_list(bool global);
void error(char* expected);
ASTnode* return_stmt();
ASTnode* arith_exp();
ASTnode* bool_exp();
NodeType relop();
ASTnode* expr_list(int* argumentCount);
ASTnode* assg_stmt();
ASTnode* if_stmt();
ASTnode* bool_exp2();

//function declarations
ASTnode* expr_list(int* argumentCount){
    ASTnode* ast_node = arith_exp();
    (*argumentCount)++;
    return ast_node;
}

ASTnode* opt_expr_list(int* argumentCount){
    ASTnode* ast_expr_list = malloc(sizeof(ASTnode));
    ast_expr_list->ntype = EXPR_LIST;
    bool commaOn = false;

    //ε because expressions can be empty
    while (curr_tok == ID || curr_tok == INTCON || commaOn){
        ASTnode* ast_node = expr_list(argumentCount);
        commaOn = false;

        //set last element to new asmt_ast
        if (ast_expr_list->child0 == NULL) {
            // First statement in the list
            ast_expr_list->child0 = ast_node; 
        } else {
            ASTnode* cur = ast_expr_list;
            while(cur->child1 != NULL){
                cur = cur->child1;
            }
            cur->child1 = malloc(sizeof(ASTnode));
            cur->child1->ntype = EXPR_LIST;
            cur->child1->child0 = ast_node;
        }
        if (curr_tok == COMMA){
            match(COMMA);
            commaOn = true;
        }
        else{
            break;
        }
    }

    return ast_expr_list;
}

ASTnode* fn_call(){
    ScopeNode* globalScope = getLastScope(symbolTable);
    char* functionID = currentID;

    //initialize astnode
    ASTnode* fn_call_ast = malloc(sizeof(ASTnode));
    fn_call_ast->ntype = FUNC_CALL;
    fn_call_ast->nameF = malloc(strlen(functionID) + 1);
    strcpy(fn_call_ast->nameF, functionID);

    //add the currentID to the symbol table if not found in globalscope and top scope
    if (chk_decl_flag){
        
        bool found = findVariableInScope(globalScope, functionID, "function");
        bool foundLocal = findFunctionOrVariableInAllScopesByNameAndType(symbolTable, functionID, "int variable");

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            error("not declared in global scope");
        }
        else if (foundLocal){
            error("variable ID declared as something else earlier");
        }
    }

    match(LPAREN);

    int argumentCount = 0;
    fn_call_ast->child0 = opt_expr_list(&argumentCount);

    if (chk_decl_flag){
        InfoNode* found = findFunctionInScopeAndGetArgCount(globalScope, functionID);
        if (found != NULL){
            if (argumentCount != *found->argCount){
                error("Number of arguments do not match");
            }
        }
        else{
            error("Could not find function a second time?");
        }
            
    }
    
    match(RPAREN);
    return fn_call_ast;
}

/*
ASTnode* arith_exp() {
    ASTnode* ast_node = malloc(sizeof(ASTnode));

    if (curr_tok == ID){
        //ast-print
        ast_node->ntype = IDENTIFIER;
        ast_node->nameF = malloc(strlen(lexeme) + 1);
        strcpy(ast_node->nameF, lexeme);

        if (chk_decl_flag){
            currentID = lexeme;
            bool found = findFunctionOrVariableInAllScopesByNameAndType(symbolTable, currentID, "int variable");
            if (found){
                match(ID);
                return ast_node;
            }
            else{
                error("variable has not been declared");
            }
        }
        else{
            match(ID);
            return ast_node;
        }
    }
    else if(curr_tok == INTCON) {
        //ast-print
        ast_node->ntype = INTCONST;
        ast_node->num = malloc(sizeof(int));
        *(ast_node->num) = atoi(lexeme);

        //add intcon into symbol table with tmpCount at end of name

        match(INTCON);
    }
    else{
        error("invalid assignment right hand side");
    }
}
*/

void arithOPAS(){
    if (curr_tok == opADD){
        match(opADD);
    }
    else if (curr_tok == opSUB){
        match(opSUB);
    }
    else{
        error("unexpected operator, expected addition or subtraction");
    }
}

ASTnode* factor(){
    ASTnode* ast_node = malloc(sizeof(ASTnode));
    if (curr_tok == ID){
        //ast-print
        ast_node->ntype = IDENTIFIER;
        ast_node->nameF = malloc(strlen(lexeme) + 1);
        strcpy(ast_node->nameF, lexeme);

        if (chk_decl_flag){
            currentID = lexeme;
            bool found = findFunctionOrVariableInAllScopesByNameAndType(symbolTable, currentID, "int variable");
            if (found){
                match(ID);
                return ast_node;
            }
            else{
                error("variable has not been declared");
            }
        }
        else{
            match(ID);
            return ast_node;
        }
    }
    else if(curr_tok == INTCON) {
        //ast-print
        ast_node->ntype = INTCONST;
        ast_node->num = malloc(sizeof(int));
        *(ast_node->num) = atoi(lexeme);

        //add intcon into symbol table with tmpCount at end of name
        //no need for above lmao

        match(INTCON);
        return ast_node;
    }
    else if (curr_tok == LPAREN){
        match(LPAREN);
        ast_node = arith_exp();
        match(RPAREN);
        return ast_node;
    }
    else if (curr_tok == opSUB){
        ast_node->ntype = UMINUS;
        match(opSUB);
        ast_node->child0 = factor();
        return ast_node;
    }
    else{
        error("could not find factor");
    }
}

ASTnode* term_tail(ASTnode* overallNode){
    if (curr_tok == opMUL){
        overallNode->ntype = MUL;
        match(opMUL);
        overallNode->child1 = factor();

        //only recurse if more nodes
        if (curr_tok == opMUL || curr_tok == opDIV){
            ASTnode* newNode = malloc(sizeof(ASTnode));
            newNode->child0 = overallNode;
            newNode = term_tail(newNode);
            return newNode;
        }
        return overallNode;
    }
    else if (curr_tok == opDIV){
        overallNode->ntype = DIV;
        match(opDIV);
        overallNode->child1 = factor();

        //only recurse if more nodes
        if (curr_tok == opMUL || curr_tok == opDIV){
            ASTnode* newNode = malloc(sizeof(ASTnode));
            newNode->child0 = overallNode;
            newNode = term_tail(newNode);
            return newNode;
        }
        return overallNode;
    }
    return overallNode;
    //can be empty
}

ASTnode* term(){
    ASTnode* ast_node = malloc(sizeof(ASTnode));
    ast_node->child0 = factor();
    ast_node = term_tail(ast_node);
    return ast_node;
}

ASTnode* getChild(ASTnode* node){
    if (node->ntype == DUMMY){
        return node->child0;
    }
    else{
        return node;
    }
}

ASTnode* arith_exp_tail(ASTnode* overallNode){
    //check at the start if its a dummy ntype then term only found an ID. if its MUL or DIV then it is a multiple operators on operands.
    //if its dummy then set the ntype here and child1
    //if its MUL or DIV then we need to create a new node set overallNode as child0 set ntype and set child1 to what we find here
    if (curr_tok == opADD){
        if (overallNode->ntype == DUMMY){
            overallNode->ntype = ADD;
            match(opADD);
        
            //will prob have to do same thing as above after term is done
            ASTnode* someNode = term();
            overallNode->child1 = getChild(someNode);

            //only recurse if more nodes
            if (curr_tok == opADD || curr_tok == opSUB){
                ASTnode* newNode = malloc(sizeof(ASTnode));
                newNode->child0 = overallNode;
                newNode = arith_exp_tail(newNode);
                return newNode;
            }
            return overallNode;
        }
        else{
            ASTnode* newHeadNode = malloc(sizeof(ASTnode));
            newHeadNode->child0 = overallNode;
            printf("not implemented\n");
        }
        
    }
    else if (curr_tok == opSUB){
        match(opSUB);

        //will prob have to do same thing as above after term is done
        term();

        //only recurse if more nodes
        if (curr_tok == opADD || curr_tok == opSUB){
            ASTnode* newNode = malloc(sizeof(ASTnode));
            arith_exp_tail(newNode);
        }
    }
    //can be empty
}

ASTnode* arith_exp() {
    //mul or div
    ASTnode* node = term();

    //add or sub
    ASTnode* perro = arith_exp_tail(node);
    return perro;
}

/*
match "=" from "ID =" then match ID or INT to end up matching SEMI to have "ID = ID || INT;"
*/
ASTnode* assg_stmt() {
    //initalize ast node
    ASTnode* ast_node = malloc(sizeof(ASTnode));
    ast_node->ntype = ASSG;
    ast_node->nameF = malloc(strlen(currentID) + 1);
    strcpy(ast_node->nameF, currentID);

    if (chk_decl_flag){
        ScopeNode* current = symbolTable;
        while (current != NULL){
            //REFORMAT to one function findVariableInScopeAndGetArgCount that does finding for you
            InfoNode* node = findVariableInScopeAndGetArgCount(current, currentID);
            if (node != NULL){
                match(opASSG);
                ASTnode* node = arith_exp();
                ast_node->child0 = node;
                match(SEMI);
                return ast_node;
            }
            current = current->next;
        }
        if (current == NULL){
            error("could not find variable");
        }
    }
    else{
        match(opASSG);
        ASTnode* node = arith_exp();
        ast_node->child0 = node;
        match(SEMI);
    }
    return ast_node;
}

/*
match keyword "return" then 
if semi found then its just "return;" 
else if it contains an id or integer then "return ID || INTCON"
*/
ASTnode* return_stmt(){
    //initialize ast node
    ASTnode* return_ast = malloc(sizeof(ASTnode));
    return_ast->ntype = RETURN;

    match(kwRETURN);
    if (curr_tok == SEMI){
        return_ast->child0 = NULL;
        match(SEMI);
    }
    else if (curr_tok == ID || curr_tok == INTCON){
        return_ast->child0 = arith_exp();
        match(SEMI);
    }
    else{
        error("invalid return statement");
    }
    return return_ast;
}

NodeType relop(){
    if (curr_tok == opEQ){
        match(opEQ);
        return EQ;
    }
    else if(curr_tok == opNE){
        match(opNE);
        return NE;
    }
    else if(curr_tok == opLE){
        match(opLE);
        return LE;
    }
    else if(curr_tok == opLT){
        match(opLT);
        return LT;
    }
    else if(curr_tok == opGE){
        match(opGE);
        return GE;
    }
    else if(curr_tok == opGT){
        match(opGT);
        return GT;
    }
    else{
        error("returning dummy instead of relop");
        return DUMMY;
    }
}

void opt_and(){
    if (curr_tok == opAND){
        match(opAND);
        bool_exp();
    }
    //can be empty
}

void bool_exp1(){
    bool_exp2();
    opt_and();
}

void opt_or(){
    if (curr_tok == opOR){
        match(opOR);
        bool_exp();
    }
    //can be empty
}

ASTnode* bool_exp(){
    bool_exp1();
    opt_or();
    return NULL;
}

/*
ASTnode* bool_exp(){
    ASTnode* exp_ast = malloc(sizeof(ASTnode));
    exp_ast->child0 = arith_exp(); 
    exp_ast->ntype = relop(); 
    exp_ast->child1 = arith_exp();
    return exp_ast;
}
*/

ASTnode* bool_exp2(){
    ASTnode* exp_ast = malloc(sizeof(ASTnode));
    exp_ast->child0 = arith_exp(); 
    exp_ast->ntype = relop(); 
    exp_ast->child1 = arith_exp();
    return exp_ast;
}

ASTnode* if_stmt(){
    ASTnode* if_ast = malloc(sizeof(ASTnode));
    match(kwIF);
    if_ast->ntype = IF;
    match(LPAREN);
    if_ast->child0 = bool_exp(); //expression is child0
    match(RPAREN);
    currentScope++;
    addScope(&symbolTable);
    if_ast->child1 = stmt(); //if expression is true
    currentScope--;
    deleteScope(&symbolTable);
    if (curr_tok == kwELSE){
        match(kwELSE);
        currentScope++;
        addScope(&symbolTable);
        if_ast->child2 = stmt();
        currentScope--;
        deleteScope(&symbolTable);
    }
    return if_ast;
}

ASTnode* while_stmt(){
    //initalize ast node
    ASTnode* while_ast = malloc(sizeof(ASTnode));
    while_ast->ntype = WHILE;

    match(kwWHILE);
    match(LPAREN);
    while_ast->child0 = bool_exp();
    match(RPAREN);
    currentScope++;
    addScope(&symbolTable);
    while_ast->child1 = stmt();
    currentScope--;
    deleteScope(&symbolTable);
    return while_ast;
}

ASTnode* stmt() {
    ASTnode* stmt_ast = NULL;
    if (curr_tok == kwIF) { //
        stmt_ast = if_stmt();
    } 
    else if (curr_tok == kwWHILE) { //
        stmt_ast = while_stmt();
    } 
    else if (curr_tok == ID) {
        currentID = lexeme;
        match(ID);
        
        if (curr_tok == LPAREN){
            stmt_ast = fn_call();
            match(SEMI);
        }
        else if (curr_tok == opASSG){
            stmt_ast = assg_stmt();
        }
        else{
            error("unkown call");
        }
    }
    else if (curr_tok == kwRETURN) { //
        stmt_ast = return_stmt();
    }
    else if (curr_tok == LBRACE) {
        match(LBRACE);
        currentScope++;
        addScope(&symbolTable);

        stmt_ast = opt_stmt_list();

        // char* popo = token_name[curr_tok];
        // printf("here2 %s\n", popo);

        match(RBRACE);
        currentScope--;
        deleteScope(&symbolTable);
    } 
    else if (curr_tok == SEMI) {
        match(SEMI);
    } 
    else {
        error("Unexpected statement");
    }
    return stmt_ast;
}

ASTnode* opt_stmt_list(){
    ASTnode* stmt_list_hd = NULL;

    //tail recursion optimization
    while(curr_tok == ID || curr_tok == kwIF || curr_tok == kwWHILE || curr_tok == kwRETURN || curr_tok == LBRACE || curr_tok == SEMI){
        ASTnode* asmt_ast = stmt();

        //set last element to new asmt_ast
        if (stmt_list_hd == NULL) {
            stmt_list_hd = malloc(sizeof(ASTnode));
            stmt_list_hd->ntype = STMT_LIST;
            // First statement in the list
            stmt_list_hd->child0 = asmt_ast; 
        } else {
            ASTnode* cur = stmt_list_hd;
            while(cur->child1 != NULL){
                cur = cur->child1;
            }
            cur->child1 = malloc(sizeof(ASTnode));
            cur->child1->ntype = STMT_LIST;
            cur->child1->child0 = asmt_ast;
        }
    }
    //no need for a list since only one statement
    return stmt_list_hd;
}

void opt_var_decls() {
    //tail recursion optimization
    while(curr_tok == kwINT) {
        var_decl();
    }
}

void formals(int* argCount) {
    type();
    currentID = lexeme;
    match(ID);

    if (chk_decl_flag){
        bool found = findVariableInScopeByName(symbolTable, currentID);

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            int* numArg = malloc(sizeof(int));
            *numArg = *argCount;
            addVariableToScope(&symbolTable, currentID, "int variable", "arg", numArg);

            //increment the amount of parameters in a function definition
            (*argCount)++;
        }
        else{
            error("variable redeclaration");
        }
    }

    if(curr_tok == COMMA) {
        match(COMMA);
        formals(argCount);
    }
}

void opt_formals(int* argCount) {
    if(curr_tok == kwINT) {
        formals(argCount);
    }
}

void type(){
    match(kwINT);
}

ASTnode* func_defn(){
    ASTnode* ast = malloc(sizeof(ASTnode));
    ast->ntype = FUNC_DEF;
    ast->nameF = currentID;
    int* argCount = malloc(sizeof(int));
    //no args
    *argCount = 0;
    
    if (chk_decl_flag){
        ScopeNode* globalScope = getLastScope(symbolTable);
        bool found = findVariableInScopeByName(globalScope, currentID);
        
        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            addVariableToScope(&symbolTable, currentID, "function", "", argCount);
        }
        else{
            error("function redeclaration");
        }
    }
    

    match(LPAREN);

    currentScope++;
    addScope(&symbolTable);

    opt_formals(argCount);
    ast->num = argCount;

    match(RPAREN);
    match(LBRACE);

    opt_var_decls();
    ast->st_ref = symbolTable->variableList;

    ast->child0 = opt_stmt_list();

    match(RBRACE);

    //printSymbolTable(symbolTable);
    if (print_ast_flag){
        print_ast(ast);
    }
    if (gen_code_flag){
        postOrderTraversal(ast);
        generateCode(ast);
        fflush(stdout);
    }

    currentScope--;
    deleteScope(&symbolTable);
    
    return ast;
}

void var_decl(){
    type();
    currentID = lexeme;
    match(ID);

    if (chk_decl_flag){
        bool found = findVariableInScopeByName(symbolTable, currentID);

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            addVariableToScope(&symbolTable, currentID, "int variable", "", NULL);
        }
        else{
            error("variable redeclaration");
        }
    }

    id_list(false);
    match(SEMI);
}

/*
to call id_list you need to match(ID) before function call
*/
void id_list(bool global) {
    //tail recursion optimization
    while (curr_tok == COMMA) {
        match(COMMA);

        currentID = lexeme;
        match(ID);

        if (chk_decl_flag){
            bool found = findVariableInScopeByName(symbolTable, currentID);

            //if it doesn't exists and the declaration is in the global scope
            if (!found){
                if (global){
                    addVariableToScope(&symbolTable, currentID, "int variable", "global", NULL);
                }
                else{
                    addVariableToScope(&symbolTable, currentID, "int variable", "", NULL);
                }
            }
            else{
                error("variable redeclaration");
            }
        }
    }
}

void prog(){
    //tail recursion optimization
    while(curr_tok == kwINT){
        type();
        currentID = lexeme;
        match(ID);

        if (curr_tok == LPAREN) {
            func_defn();
            //printSymbolTable(symbolTable);
        } else {

            if (chk_decl_flag){
                bool found = findVariableInScopeByName(symbolTable, currentID);

                //if it doesn't exists and the declaration is in the global scope
                if (!found){
                    addVariableToScope(&symbolTable, currentID, "int variable", "global", NULL);
                }
                else{
                    error("variable redeclaration");
                }
            }

            id_list(true);
            match(SEMI);
            if (gen_code_flag){
                generateGlobalVariables();
            }
        }

        //printSymbolTable(symbolTable);
    }
    return;
}

void error(char* expected) {
    fprintf(stderr, "ERROR: %s in LINE %d\n", expected, line_num);
    exit(1);
}

void match(Token expected) {
    if (curr_tok == expected) {
        curr_tok = get_token();
    }
    else {
        if (expected == EOF){
            error("EOF");
        }
        else{
            char* expectedTokenStr = token_name[expected];
            error(expectedTokenStr);
        }
    }
}

int parse(){
    curr_tok = get_token();
    int* argCountForPrintF = malloc(sizeof(int));

    if (chk_decl_flag){
        //global scope
        addScope(&symbolTable);
        currentScope++;
        
        if (gen_code_flag){
            *argCountForPrintF = 1;
            addVariableToScope(&symbolTable, "println", "function", "", argCountForPrintF);
            char* printlnF = ".data\n_nl: .asciiz \"\\n\"\n.text\n.globl _println\n_println:\n\taddiu $sp, $sp, -8\n\tsw $fp, 4($sp)\n\tsw $ra, 0($sp)\n\tmove $fp, $sp\n\tlw   $a0, 8($fp)\n\tli   $v0, 1\n\tsyscall\n\tla   $a0, _nl\n\tli   $v0, 4\n\tsyscall\n\tmove $sp, $fp\n\tlw $ra, 0($sp)\n\tlw $fp, 4($sp)\n\taddiu $sp, $sp, 8\njr   $ra\n\n";
            fprintf(stdout, "%s", printlnF);
            fflush(stdout);
        }
    }
    
    //start
    prog();

    //end
    match(EOF);
    return 0;
}

/*
bool_exp : bool_exp1 opt_or
opt_or : "||" bool_expr 
| ε
bool_exp1 : bool_exp2 opt_and
opt_and : "&&" bool_expr
| ε
bool_exp2 : arith_exp relop arith_exp

*/
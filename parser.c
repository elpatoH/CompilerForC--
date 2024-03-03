/*
name: Daniel Gil
class: cs453
description: syntax analyzer for c--
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "symbolTable.h"
#include "ast.h"

extern int get_token();
extern int line_num;
extern char* lexeme;
extern int chk_decl_flag;
Token curr_tok;

int currentScope = 0;
ScopeNode* symbolTable = NULL;
char* currentID;

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
void fn_call();
void opt_expr_list(int* argumentCount);
void formals(int* argCount);
void var_decl();
void id_list();
void error(char* expected);
void return_stmt();
ASTnode* arith_exp();
ASTnode* bool_exp();
NodeType relop();
void expr_list(int* argumentCount);
void assg_stmt();
ASTnode* if_stmt();


//function declarations
void expr_list(int* argumentCount){
    arith_exp();
    (*argumentCount)++;
    if (curr_tok == COMMA){
        match(COMMA);
        expr_list(argumentCount);
    }
}

void opt_expr_list(int* argumentCount){
    //ε because expressions can be empty
    if (curr_tok == ID || curr_tok == INTCON){
        expr_list(argumentCount);
    }
    return;
}

void fn_call(){
    // currentID = lexeme;
    // match(ID);
    ScopeNode* globalScope = getLastScope(symbolTable);
    char* functionID = currentID;

    //add the currentID to the symbol table if not found in globalscope and top scope
    if (chk_decl_flag){
        
        bool found = findVariableInScope(globalScope, functionID, "function", "");
        bool foundLocal = findFunctionOrVariableInAllScopesByNameAndType(symbolTable, functionID, "int variable");

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            error("function redeclaration or not declared in global scope");
        }
        else if (foundLocal){
            error("variable ID declared as something else earlier");
        }
    }

    match(LPAREN);

    int argumentCount = 0;
    opt_expr_list(&argumentCount);

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
}

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
        //might need to delete this
        match(ID);
    }
    else if(curr_tok == INTCON) {
        //ast-print
        ast_node->ntype = INTCONST;
        ast_node->num = malloc(sizeof(int));
        *(ast_node->num) = atoi(lexeme);

        match(INTCON);
    }
    else{
        error("invalid assignment right hand side");
    }
    return ast_node;
}

/*
match "=" from "ID =" then match ID or INT to end up matching SEMI to have "ID = ID || INT;"
*/
void assg_stmt() {
    if (chk_decl_flag){
        ScopeNode* current = symbolTable;
        while (current != NULL){
            InfoNode* node = findVariableInScopeAndGetArgCount(current, currentID);
            if (node != NULL){
                match(opASSG);
                arith_exp();
                match(SEMI);
                return;
            }
            current = current->next;
        }
        if (current == NULL){
            error("could not find variable");
        }
    }
    else{
        match(opASSG);
        arith_exp();
        match(SEMI);
    }
}

/*
match keyword "return" then 
if semi found then its just "return;" 
else if it contains an id or integer then "return ID || INTCON"
*/
void return_stmt(){
    match(kwRETURN);
    if (curr_tok == SEMI){
        match(SEMI);
    }
    else if (curr_tok == ID || curr_tok == INTCON){
        arith_exp();
        match(SEMI);
    }
    else{
        error("invalid return statement");
    }
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
    return DUMMY;
}

ASTnode* bool_exp(){
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

void while_stmt(){
    match(kwWHILE);
    match(LPAREN);
    bool_exp();
    match(RPAREN);
    currentScope++;
    addScope(&symbolTable);
    stmt();
    currentScope--;
    deleteScope(&symbolTable);
}

ASTnode* stmt() {
    ASTnode* stmt_ast = NULL;
    if (curr_tok == kwIF) {
        stmt_ast = if_stmt();
    } 
    else if (curr_tok == kwWHILE) {
        while_stmt();
    } 
    else if (curr_tok == ID) {
        currentID = lexeme;
        match(ID);
        
        if (curr_tok == LPAREN){
            fn_call();
            match(SEMI);
        }
        else if (curr_tok == opASSG){
            assg_stmt();
        }
        else{
            error("unkown call");
        }
    }
    else if (curr_tok == kwRETURN) {
        return_stmt();
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
    ASTnode* stmt_list_hd = malloc(sizeof(ASTnode));
    stmt_list_hd->ntype = STMT_LIST;


    //tail recursion optimization
    while(curr_tok == ID || curr_tok == kwIF || curr_tok == kwWHILE || curr_tok == kwRETURN || curr_tok == LBRACE || curr_tok == SEMI){
        ASTnode* asmt_ast = stmt();

        //set last element to new asmt_ast
        if (stmt_list_hd->child0 == NULL) {
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
    //no need for a list since only one statement or also could be empty block
    if (stmt_list_hd->child1 == NULL){
        return stmt_list_hd->child0;
    }
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
            addVariableToScope(&symbolTable, currentID, "int variable", "", NULL);

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
    
    if (chk_decl_flag){
        ScopeNode* globalScope = getLastScope(symbolTable);
        bool found = findVariableInScopeByName(globalScope, currentID);
        
        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            addVariableToScope(&symbolTable, currentID, "function", "", argCount);
        }
        else{
            error("function redeclaration or not declared in global scope");
        }
    }
    

    match(LPAREN);

    currentScope++;
    addScope(&symbolTable);

    opt_formals(argCount);
    ast->num = argCount;

    match(RPAREN);
    match(LBRACE);

    //might need to allocate new memory for variable list
    opt_var_decls();
    ast->st_ref = symbolTable->variableList;

    ASTnode* stmt_list_head = opt_stmt_list();
    ast->child0 = stmt_list_head;

    match(RBRACE);

    //printSymbolTable(symbolTable);
    if (print_ast_flag){
        print_ast(ast);
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

    id_list();
    match(SEMI);
}

/*
to call id_list you need to match(ID) before function call
*/
void id_list() {
    //tail recursion optimization
    while (curr_tok == COMMA) {
        match(COMMA);

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
    }
}

void prog(){
    //tail recursion optimization
    while(curr_tok == kwINT){
        type();
        currentID = lexeme;
        match(ID);

        if (curr_tok == LPAREN) {
            ASTnode* popo = func_defn();
            //printSymbolTable(symbolTable);
        } else {

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

            id_list();
            match(SEMI);
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

    if (chk_decl_flag){
        //global scope
        addScope(&symbolTable);
        currentScope++;
    }
    
    //start
    prog();

    //end
    match(EOF);
    return 0;
}
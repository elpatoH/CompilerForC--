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

extern int get_token();
extern int line_num;
extern char* lexeme;
extern int chk_decl_flag;
Token curr_tok;

int currentScope = 0;
ScopeNode* symbolTable = NULL;
char* currentID;

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
void func_defn();
void type();
void opt_formals();
void opt_var_decls();
void opt_stmt_list();
void stmt();
void fn_call();
void opt_expr_list(int* argC);
void formals();
void var_decl();
void id_list();
void error(char* expected);
void return_stmt();
void arith_exp();
void bool_exp();
void relop();
void expr_list(int* argC);
void assg_stmt();


//function declarations
void expr_list(int* argC){
    arith_exp();
    (*argC)++;
    if (curr_tok == COMMA){
        match(COMMA);
        expr_list(argC);
    }
}

void opt_expr_list(int* argC){
    //ε because expressions can be empty
    if (curr_tok == ID || curr_tok == INTCON){
        expr_list(argC);
    }
    return;
}

/*
need one function that looks at the variables in the current scope and see if it was declared again
*/
void fn_call(){
    ScopeNode* globalScope = getLastScope(symbolTable);
    // currentID = lexeme;
    // match(ID);

    //REFACTOR
    //REUSE
    //add the currentID to the symbol table if not found in globalscope and top scope
    if (chk_decl_flag){
        bool found = findVariableInScope(globalScope, currentID, "function", "");
        //this might break because it will also look in the global for an int variable but it should be fine since
        //there is a check for doubleid when created.
        bool foundNewDeclaration = findFunctionOrVariableInAllScopesByNameAndType(symbolTable, currentID, "int variable");

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            error("not declared in global scope");
        }
        else if (foundNewDeclaration){
            error("variable ID declared as something else earlier");
        }
    }

    match(LPAREN);

    int argCount = 0;
    opt_expr_list(&argCount);
    
    if (chk_decl_flag){
        int expectedArgCount;
        //TODO
        //functions can only live in global scope
        //might need to use a different function here and delete it since current one searches everywhere
        bool declared = findFunctionInScopeAndGetArgCount(globalScope, currentID, &expectedArgCount);
        if (!declared) {
            error("Function not declared");
        } 
        else if (argCount != expectedArgCount) {
            error("Argument count is different than expected");
        }
    }

    match(RPAREN);
}

void arith_exp() {
    if (curr_tok == ID){
        match(ID);
    }
    else if(curr_tok == INTCON) {
        match(INTCON);
    }
    else{
        error("invalid assignment right hand side");
    }
}

/*
match "=" from "ID =" then match ID or INT to end up matching SEMI to have "ID = ID || INT;"
*/
void assg_stmt() {
    //REUSE
    //when more variable types are added add more found 
    if (chk_decl_flag){
        bool found = findFunctionOrVariableInAllScopesByNameAndType(symbolTable, currentID, "int variable");
        if (!found){
            error("left hand variable is not declared");
        }
    }
    match(opASSG);
    arith_exp();
    match(SEMI);
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

void relop(){
    if (curr_tok == opEQ){
        match(opEQ);
    }
    else if(curr_tok == opNE){
        match(opNE);
    }
    else if(curr_tok == opLE){
        match(opLE);
    }
    else if(curr_tok == opLT){
        match(opLT);
    }
    else if(curr_tok == opGE){
        match(opGE);
    }
    else if(curr_tok == opGT){
        match(opGT);
    }
}

void bool_exp(){
    arith_exp(); 
    relop(); 
    arith_exp();
}

void if_stmt(){
    match(kwIF);
    match(LPAREN);
    bool_exp();
    match(RPAREN);
    stmt();
    if (curr_tok == kwELSE){
        match(kwELSE);
        stmt();
    }
}

void while_stmt(){
    match(kwWHILE);
    match(LPAREN);
    bool_exp();
    match(RPAREN);
    stmt();
}

void stmt() {
    if (curr_tok == kwIF) {
        if_stmt();
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
        opt_stmt_list();
        match(RBRACE);
    } 
    else if (curr_tok == SEMI) {
        match(SEMI);
    } 
    else {
        error("Unexpected statement");
    }
}

void opt_stmt_list(){
    //tail recursion optimization
    while(curr_tok == ID || curr_tok == kwIF || curr_tok == kwWHILE || curr_tok == kwRETURN || curr_tok == LBRACE || curr_tok == SEMI){
        stmt();
    }
    return;
}

void opt_var_decls() {
    //tail recursion optimization
    while(curr_tok == kwINT) {
        var_decl();
    }
}

void formals() {
    type();
    currentID = lexeme;
    match(ID);

    if (chk_decl_flag){
        bool found = findVariableInScopeByName(symbolTable, currentID);

        //CHANGE
        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            addVariableToScope(&symbolTable, currentID, "int variable", "", 0);
        }
        else{
            error("variable redeclaration");
        }
    }

    if(curr_tok == COMMA) {
        match(COMMA);
        formals();
    }
}

void opt_formals() {
    if(curr_tok == kwINT) {
        formals();
    }
}

void type(){
    match(kwINT);
}

void func_defn(){
    if (chk_decl_flag){
        ScopeNode* globalScope = getLastScope(symbolTable);
        bool found = findVariableInScopeByName(globalScope, currentID);

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            addVariableToScope(&symbolTable, currentID, "function", "", 0);
        }
        else{
            error("function redeclaration");
        }
    }

    match(LPAREN);

    currentScope++;
    addScope(&symbolTable);
    
    opt_formals();
    match(RPAREN);
    match(LBRACE);
    opt_var_decls();
    opt_stmt_list();
    match(RBRACE);

    currentScope--;
    deleteScope(&symbolTable);
    
    return;
}

void var_decl(){
    type();
    currentID = lexeme;
    match(ID);

    if (chk_decl_flag){
        bool found = findVariableInScopeByName(symbolTable, currentID);

        //if it doesn't exists and the declaration is in the global scope
        if (!found){
            addVariableToScope(&symbolTable, currentID, "int variable", "", 0);
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
                addVariableToScope(&symbolTable, currentID, "int variable", "", 0);
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
        } else {

            if (chk_decl_flag){
                bool found = findVariableInScopeByName(symbolTable, currentID);

                //if it doesn't exists and the declaration is in the global scope
                if (!found){
                    addVariableToScope(&symbolTable, currentID, "int variable", "", 0);
                }
                else{
                    error("variable redeclaration");
                }
            }

            id_list();
            match(SEMI);
        }
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
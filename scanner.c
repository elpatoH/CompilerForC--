/*
name: Daniel Gil
class: csc453
description: contains the get_token() implementation

keep track of line numbers
*/

#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

char* lexeme;
char buffer[MAX_BUFFER_SIZE];
int line_num = 1;

/*
add skippable characters here
*/
void skip_whitespace_and_comments(){
    int ch;

    while ((ch = getchar()) != EOF) {
        //increase line number
        if (ch == '\n'){
            line_num++;
            continue;
        }
        //skip invisible chars
        else if (ch == ' ' || ch == '\t' || ch == '\r') {
            continue;
        }
        
        //check for multiline comment
        else if (ch == '/') {
            ch = getchar();
            if (ch == '*'){
                //in multiline comment  != EOF && !((ch = getchar()) == '*' && (ch = getchar()) == '/')
                while ((ch = getchar()) != EOF) {
                    if (ch == '\n'){
                        line_num++;
                        continue;
                    }
                    else if (ch == '*'){
                        if ((ch = getchar()) == '/'){
                            break;
                        }
                        else{
                            ungetc(ch, stdin);
                        }
                    }
                }
            }
            else {
                ungetc(ch, stdin);
                ungetc('/', stdin);
                return;
            }
        }
        else {
            ungetc(ch, stdin);
            return;
        }
    }
    if(ch == EOF){
        ungetc(EOF, stdin);
    }
}

/*
add keywords here
*/
int keywd_or_id(char* buffer){
    if (strcmp(buffer, "int") == 0) {
        return kwINT;
    }
    if (strcmp(buffer, "if") == 0) {
        return kwIF;
    }
    if (strcmp(buffer, "else") == 0) {
        return kwELSE;
    }
    if (strcmp(buffer, "while") == 0) {
        return kwWHILE;
    }
    if (strcmp(buffer, "return") == 0) {
        return kwRETURN;
    }
    //add more keywords in the future


    return ID;
}

int get_token(){
    char ch;

    skip_whitespace_and_comments();
    ch = getchar();
    if (ch == EOF) {return EOF;}

    /*no future read chars*/
    if (ch == '(') {lexeme = "("; return LPAREN;}
    if (ch == ')') {lexeme = ")"; return RPAREN;}

    if (ch == '{') {lexeme = "{"; return LBRACE;}
    if (ch == '}') {lexeme = "}"; return RBRACE;}

    if (ch == ',') {lexeme = ","; return COMMA;}
    if (ch == ';') {lexeme = ";"; return SEMI;}

    /*future read chars*/
    if (ch == '<') {
        //peek next char
        ch = getchar();

        //if next char is '=' then we found lexeme <=
        if (ch == '=') {lexeme = "<="; return opLE;}
        //else lexeme is now only <
        else{ungetc(ch, stdin); lexeme = "<"; return opLT;}
    }
    if (ch == '>') {
        ch = getchar();

        if (ch == '=') {lexeme = ">="; return opGE;}
        else{ungetc(ch, stdin); lexeme = ">"; return opGT;}
    }
    if (ch == '=') {
        ch = getchar();

        if (ch == '=') {lexeme = "=="; return opEQ;}
        else {ungetc(ch, stdin); lexeme = "="; return opASSG;}
    }

    /*will need to add operations like ++ to it in the future*/
    if (ch == '+') {
        lexeme = "+"; return opADD;
    }
    if (ch == '-') {
        lexeme = "-"; return opSUB;
    }
    if (ch == '*') {
        lexeme = "*"; return opMUL;
    }
    if (ch == '/') {
        lexeme = "/"; return opDIV;
    }

    /*comparisons*/
    if (ch == '!') {
        ch = getchar();

        if (ch == '=') {lexeme = "!="; return opNE;}
        else {ungetc(ch, stdin); lexeme = "!"; return opNOT;}
    }
    if (ch == '&') {
        ch = getchar();

        if (ch == '&') {lexeme = "&&"; return opAND;}
        else {ungetc(ch, stdin); lexeme = "&"; return UNDEF;}
    }
    if (ch == '|') {
        ch = getchar();

        if (ch == '|') {lexeme = "||"; return opOR;}
        else {ungetc(ch, stdin); lexeme = "|"; return UNDEF;}
    }

    /*digits*/
    if (isdigit(ch) != 0) {
        char* ptr = buffer;

        //buffer[0] = ch and move ptr to buffer[1]
        *ptr++ = ch;

        //read subsequent chars if any
        ch = getchar();
        while (isdigit(ch)) {
            *ptr++ = ch;
            ch = getchar();
        }
        *ptr = '\0';

        if (ch != EOF) {
            ungetc(ch, stdin);
        }
        
        lexeme = buffer;
        return INTCON;
    }

    /*keywords or identifiers*/
    if (isalpha(ch)) {
        char* ptr = buffer;

        //buffer[0] = ch and move ptr to buffer[1]
        *ptr++ = ch;

        ch = getchar();
        while (isalnum(ch) || ch == '_') {
            *ptr++ = ch;
            ch = getchar();
        }
        *ptr = '\0';

        if (ch != EOF) {
            ungetc(ch, stdin);
        }

        lexeme = strdup(buffer);
        return keywd_or_id(buffer);
    }

    //undefined behavior
    lexeme = &ch;
    return UNDEF;
}
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define the InfoNode structure
typedef struct InfoNode {
    char name[100];
    char type[100];
    char info[256];
    int* argCount;  
    struct InfoNode* next;
} InfoNode;

// Define the ScopeNode structure
typedef struct ScopeNode {
    struct InfoNode* variableList; // Points to the first variable in this scope
    struct ScopeNode* next;
} ScopeNode;

// Declarations of functions used in the symbol table
void addVariableToScope(ScopeNode** head, const char* name, const char* type, const char* info, int* argCount);
void addScope(ScopeNode** head);
void deleteScope(ScopeNode** head);
void printSymbolTable(ScopeNode* head);
bool findVariableInScope(ScopeNode* head, const char* name, const char* type, const char* info);
InfoNode* findFunctionInScopeAndGetArgCount(ScopeNode* head, const char* name);
bool findFunctionOrVariableInAllScopesByNameAndType(ScopeNode* head, const char* name, const char* type);
InfoNode* findVariableInScopeAndGetArgCount(ScopeNode* head, const char* name);
bool findVariableInScopeByName(ScopeNode* head, const char* name);
bool findFunctionInAllScopes(ScopeNode* head, const char* name, int* argCount);
ScopeNode* getLastScope(ScopeNode* head);

#endif
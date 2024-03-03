#include "symbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool compareVariablesByName(const InfoNode* variable, const char* name);
bool compareVariables(const InfoNode* variable, const char* name, const char* type);
bool findFunctionOrVariableInAllScopesByNameAndTypeHelper(ScopeNode* head, const char* name, const char* type);

/*
create new infonode
*/
InfoNode* createVariableNode(const char* name, const char* type, const char* info, int* argCount) {
    InfoNode* newNode = malloc(sizeof(InfoNode));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    strncpy(newNode->name, name, sizeof(newNode->name) - 1);
    strncpy(newNode->type, type, sizeof(newNode->type) - 1);
    strncpy(newNode->info, info, sizeof(newNode->info) - 1);
    newNode->argCount = argCount;
    newNode->next = NULL;
    return newNode;
}

/*
add variable node to start of list
*/
void addVariableToScope(ScopeNode** head, const char* name, const char* type, const char* info, int* argCount) {
    if (head == NULL || *head == NULL) {
        return;
    }

    ScopeNode* firstScope = *head;
    InfoNode* newNode = createVariableNode(name, type, info, argCount);

    newNode->next = firstScope->variableList;
    firstScope->variableList = newNode;
}

bool findFunctionOrVariableInAllScopesByNameAndType(ScopeNode* head, const char* name, const char* type) {
    ScopeNode* currentScope = head;
    while (currentScope != NULL) {
        if (findFunctionOrVariableInAllScopesByNameAndTypeHelper(currentScope, name, type)) {
            return true;
        }
        currentScope = currentScope->next;
    }
    return false;
}

bool findFunctionOrVariableInAllScopesByNameAndTypeHelper(ScopeNode* head, const char* name, const char* type) {
    if (head == NULL) return false;
    InfoNode* variable = head->variableList;
    while (variable != NULL) {
        if (compareVariables(variable, name, type)) {
            return true;
        }
        variable = variable->next;
    }
    return false;
}

bool findFunctionInAllScopes(ScopeNode* head, const char* name, int* argCount) {
    ScopeNode* currentScope = head;
    while (currentScope != NULL) {
        if (findFunctionInScopeAndGetArgCount(currentScope, name)) {
            return true;
        }
        currentScope = currentScope->next;
    }
    return false;
}

/*
find a function declaration by name and argcount
*/
InfoNode* findFunctionInScopeAndGetArgCount(ScopeNode* head, const char* name) {
    if (head == NULL) return NULL;
    InfoNode* variable = head->variableList;
    while (variable != NULL) {
        if (compareVariablesByName(variable, name) && strcmp(variable->type, "function") == 0) {
            return variable;
        }
        variable = variable->next;
    }
    return NULL;
}

InfoNode* findVariableInScopeAndGetArgCount(ScopeNode* head, const char* name) {
    if (head == NULL) return NULL;
    InfoNode* variable = head->variableList;
    while (variable != NULL) {
        if (compareVariablesByName(variable, name) && strcmp(variable->type, "int variable") == 0) {
            return variable;
        }
        variable = variable->next;
    }
    return NULL;
}

/*
create new scopenode
*/
ScopeNode* createScopeNode() {
    ScopeNode* newNode = malloc(sizeof(ScopeNode));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    newNode->variableList = NULL;
    newNode->next = NULL;
    return newNode;
}

/*
add scopenode to table
*/
void addScope(ScopeNode** head) {
    ScopeNode* newNode = createScopeNode();
    newNode->next = *head;
    *head = newNode;
}

/*
delete the last scope added
*/
void deleteScope(ScopeNode** head) {
    if (head == NULL || *head == NULL) return;

    ScopeNode* toDelete = *head;
    *head = (*head)->next;

    InfoNode* variable = toDelete->variableList;
    while (variable != NULL) {
        InfoNode* temp = variable;
        variable = variable->next;
        free(temp);
    }

    free(toDelete);
}


/*
print symbol table
*/
void printSymbolTable(ScopeNode* head) {
    ScopeNode* scope = head;
    int scopeIndex = 0;
    while (scope != NULL) {
        printf("Scope %d:\n", scopeIndex);
        InfoNode* variable = scope->variableList;
        while (variable != NULL) {
            if (variable->argCount == NULL){
                printf("  Name: %s, Type: %s, Info: %s\n", variable->name, variable->type, variable->info);
            }
            else{
                printf("  Name: %s, Type: %s, Info: %s, argCount: %d\n", variable->name, variable->type, variable->info, *variable->argCount);
            }
            variable = variable->next;
        }
        scope = scope->next;
        scopeIndex++;
    }
}

/*
helper function for comparing variables
*/
bool compareVariables(const InfoNode* variable, const char* name, const char* type) {
    return strcmp(variable->name, name) == 0 && strcmp(variable->type, type) == 0;
}

/*
look for variable in passed in scope
*/
bool findVariableInScope(ScopeNode* head, const char* name, const char* type) {
    if (head == NULL) {
        return false;
    }

    InfoNode* variable = head->variableList;
    while (variable != NULL) {
        if (compareVariables(variable, name, type)) {
            return true;
        }
        variable = variable->next;
    }
    return false;
}

/*
helper function for comparing variables by name
*/
bool compareVariablesByName(const InfoNode* variable, const char* name) {
    return strcmp(variable->name, name) == 0;
}


/*
look for variable in passed in scope by name
*/
bool findVariableInScopeByName(ScopeNode* head, const char* name) {
    if (head == NULL) {
        return false;
    }

    InfoNode* variable = head->variableList;
    while (variable != NULL) {
        if (compareVariablesByName(variable, name)) {
            return true;
        }
        variable = variable->next;
    }
    return false;
}

/*
get the global scope
*/
ScopeNode* getLastScope(ScopeNode* head) {
    if (head == NULL) {
        return NULL;
    }

    ScopeNode* current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    return current;
}
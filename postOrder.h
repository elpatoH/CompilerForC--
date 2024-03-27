#ifndef POST_ORDER_H
#define POST_ORDER_H

#include "ast.h"

static int label_num = 0;
static int name_num = 0;
extern ScopeNode* symbolTable;

void generateCode(ASTnode* tree);
void postOrderTraversal(ASTnode* node);
void codeGen_expr(ASTnode* e);
//write following functions and keep on writing the codeGen_stmt,
//  I believe codeGen_expr will not be implemented until next milestone
//finish writing codeGen_stmt by implementing each case in switch
//  start reading from slide 13 to get back to where you left off.
//  basically we are writing codegenstmt function to populate the new fields in symbol table,
InfoNode* newtemp(char* type);
Quad* newinstr(NodeType op, void* src1, void* src2, void* dest);
Quad* newlabel();
void visit(ASTnode* node);


#endif
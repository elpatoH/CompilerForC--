#include "ast.h"


NodeType ast_node_type(void* ptr) {
    ASTnode* ast = (ASTnode*) ptr;
    if (ast == NULL){
        printf("null pointer\n");
        exit(1);
    }
    return ast->ntype;
}

/*
returns the name of the function;
*/
char* func_def_name(void* ptr) {
    ASTnode *node = (ASTnode*) ptr;
    if (node->ntype != FUNC_DEF) {printf("not func def.\n"); exit(1);};
    return node->nameF;
}

/*
returns the number of formal parameters for the function;
*/
int func_def_nargs(void* ptr) {
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != FUNC_DEF) {printf("not func def.\n"); exit(1);};
    return *node->num;
}

/*
returns a pointer to the AST of the body of the function
*/
void* func_def_body(void* ptr) {
    ASTnode *node = (ASTnode*) ptr;
    if (node->ntype != FUNC_DEF) {printf("not func def.\n"); exit(1);};
    return node->child0;
}

/*
returns the name of the nth parameter
*/
char* func_def_argname(void* ptr, int n){
    ASTnode *node = (ASTnode*) ptr;
    if (node->ntype != FUNC_DEF) {printf("not func def.\n"); exit(1);};
    if (n <= 0){printf("need n > 0."); exit(1);}
    //grab number of arguments for function definition in ptr.
    if (n > func_def_nargs(node)) {printf("n greater than actual num args.\n"); exit(1);}

    //return the name of the n parameter
    InfoNode* cur = node->st_ref;
    int index = 1;
    while (index <= func_def_nargs(node) - n){
        cur = cur->next;
        index++;
    }
    return cur->name;
}

/*
 * ptr: pointer to an AST node for a function call; func_call_callee() returns 
 * a pointer to a string that is the name of the function being called.
 */
char* func_call_callee(void* ptr){
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != FUNC_CALL) {printf("not func def.\n"); exit(1);};
    return "/somefunc/()";
}

/*
 * ptr: pointer to an AST node for a function call; func_call_args() returns 
 * a pointer to the AST that is the list of arguments to the call.
 */
void* func_call_args(void *ptr){
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != FUNC_CALL) {printf("not func def.\n"); exit(1);};
    return node->st_ref->name;
}

void *stmt_list_head(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != STMT_LIST) {printf("not statement list.\n"); exit(1);};
    return node->child0;
}

void *stmt_list_rest(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != STMT_LIST) {printf("not statement list.\n"); exit(1);};
    return node->child1;
}

void *expr_list_head(void *ptr)
{
    return NULL;
}

void *expr_list_rest(void *ptr)
{
    return NULL;
}

char *expr_id_name(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != IDENTIFIER) {printf("not an identifier.\n"); exit(1);};
    return node->nameF;
}

int expr_intconst_val(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != INTCONST) {printf("not integer constant.\n"); exit(1);};
    return *node->num;
}

void *expr_operand_1(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    return node->child0;
}

void *expr_operand_2(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    return node->child1;
}

void *stmt_if_expr(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != IF) {printf("not if statement.\n"); exit(1);};
    return node->child0;
}

void *stmt_if_then(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != IF) {printf("not if statement.\n"); exit(1);};
    return node->child1;
}

void *stmt_if_else(void *ptr)
{
    ASTnode* node = (ASTnode*) ptr;
    if (node->ntype != IF) {printf("not if statement.\n"); exit(1);};
    return node->child2;
}

char *stmt_assg_lhs(void *ptr)
{
    return NULL;
}

void *stmt_assg_rhs(void *ptr)
{
    return NULL;
}

void *stmt_while_expr(void *ptr)
{
    return NULL;
}

void *stmt_while_body(void *ptr)
{
    return NULL;
}

void *stmt_return_expr(void *ptr)
{
    return NULL;
}

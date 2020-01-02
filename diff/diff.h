#ifndef DIFF_H

#define DIFF_H
/*
grammatic rules:
G ::= E#

E ::= T {[+ -]T}*

T ::= power {[/ *]power}* 

power ::= P{^P}?

P ::= (E) | number | var | function

function ::= cos(E) | sin(E) | ln(E) | exp(E) | arcsin(E) | arccos(E) | arctg(E) | tg(E) | sh(E) | ch(E) | sqrt(E)

var ::= [A-Z]

number ::= {-}?[0-9]+
*/


Node *get_G(char *str);
Node *get_T();
Node *get_P();
Node *get_number();
Node *get_E();
Node *get_power();
Node *get_function();
Node *get_var();

Tree *diff_tree(Tree *tree, char var);
Node *diff_node(Node *node);
Node *diff_node_op(Node *node);
Node *diff_node_func(Node *node);
Node *copy_node(Node *node);

Node *_ADD(Node *l_node, Node *r_node);
Node *_SUB(Node *l_node, Node *r_node);
Node *_MUL(Node *l_node, Node *r_node);
Node *_DIV(Node *l_node, Node *r_node);
Node *_POWER(Node *arg_l, Node *arg_r);
Node *_FUNC(enum func func_name, Node *arg);
Node *_NUM(int num);
Node *_VAR(char var);

Node *simplify_node(Node *node);
Node *simplify_node_add_sub(Node *node);
Node *simplify_node_mul(Node *node);
Tree *simplify_tree(Tree *tree);

#endif

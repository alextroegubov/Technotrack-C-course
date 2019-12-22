#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "tree.h"

Node *get_G(char *str);
Node *get_mul_div();
Node *get_P();
Node *get_number();
Node *get_add_sub();
Node *get_W();
Node *get_function();
Node *get_var();
Node *get_D();

int power(int e, int x){
	int res = 1;
	if(x != 0)
		for(int i = x; i > 0; i--)
			res *= e;

	return res;
}


/*
EX+: {17, 127*(9-1), ..}
EX_: {13^2 + 6, 44}

grammatic rules:

G ::= add_sub#

add_sub ::= mul_div {[+ -]mul_div}*

mul_div ::= W {[/ *]W}* 

W :: = P{^P}?

P ::= (add_sub) | number | var | function

function ::= cos(add_sub) | sin(add_sub) | 

var ::= [A-Z]

number ::= [0-9]+

*/

char *s = NULL;

Node *get_G(char *str){
	
	s = str;

	Node *val = get_add_sub();
	assert(*s == '\0');

	return val;
}

Node *get_function(){
	Node *val1 = NULL;

	if((*s == 's' && *(s+1) == 'i' && *(s+2) == 'n' && *(s+3) == '(') ||
		(*s == 'c' && *(s+1) == 'o' && *(s+2) == 's' && *(s+3) == '(')){
		char op = *s;
		s += 4;
		Node *val2 = get_add_sub();
		assert(*s == ')');
		s++;

		if(op == 's'){
			val1 = create_node(FUNC);
			val1->left = val2;
			val1->right = NULL;
			((Info_func*)(val1->info))->func = SIN;
		}
		else{
			val1 = create_node(FUNC);
			val1->left = val2;
			val1->right = NULL;
			((Info_func*)(val1->info))->func = COS;
		}

	}
//	else
//		val1 = get_P();

	return val1;
}

Node *get_add_sub(){

	Node *val1 = get_mul_div();
	Node *val2 = NULL;
	Node *new_node = NULL;

	while(*s == '+' || *s == '-'){
		char op = *s;
		s++;
		val2 = get_mul_div();

		if(op == '+'){
			new_node = create_node(OP);
			new_node->left = val1;
			new_node->right = val2;
			((Info_op*)(new_node->info))->op = ADD;
			val1 = new_node;
		}
		else{
			new_node = create_node(OP);
			new_node->left = val1;
			new_node->right = val2;
			((Info_op*)(new_node->info))->op = SUB;
			val1 = new_node;
		}
	}
	return val1;
}

Node *get_mul_div(){
	Node *val1 = get_W();
	Node *new_node = NULL;

	while(*s == '*' || *s == '/'){		
		char op = *s;
		s++;
		Node *val2 = get_W();

		if(op == '*'){
			new_node = create_node(OP);
			new_node->left = val1;
			new_node->right = val2;
			((Info_op*)(new_node->info))->op = MUL;
			val1 = new_node;
		}
		else{
			new_node = create_node(OP);
			new_node->left = val1;
			new_node->right = val2;
			((Info_op*)(new_node->info))->op = DIV;
			val1 = new_node;
		}
	}
	return val1;
}


Node *get_W(){
	Node *val1 = get_P();

	if(*s == '^'){
		s++;
		Node *val2 = get_P();		
		Node *new_node = create_node(OP);
		new_node->left = val1;
		new_node->right = val2;
		((Info_op*)(new_node->info))->op = POW;
		val1 = new_node;
	}

	return val1;
}

Node *get_P(){
	
	if(*s == '('){		
		s++;
		Node *val = get_add_sub();		
		assert(*s == ')');
		s++;
		return val;
	}
	else if('0' <= *s && *s <= '9')
		return get_number();
	else if('A' <= *s && *s <= 'Z') 
		return get_var();
	else 
		return get_function();
}

Node *get_var(){
	char val = 0;
	Node *new_node = NULL;

	if('A' <= *s && *s <= 'Z'){
		val = *s;
		s++;
		new_node = create_node(VAR);
		((Info_var*)(new_node->info))->var = val;
	}

	return new_node;
}

Node *get_number(){
	int val = 0;
	
	do{		
		val = val * 10 + *s - '0';		
		s++;

	}while('0' <= *s && *s <= '9');

	Node *new_node = create_node(NUM);
	((Info_num*)(new_node->info))->num = val;

	return new_node;
}

int main(){

	char str[100] = {'\0'};
	scanf("%s", str);

	Tree *tree = tree_create();
	
	tree->root = get_G(str);

	tree_save_graph(tree, "tree.dot");


	return 0;
}

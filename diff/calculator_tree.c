#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "tree.h"

Node *get_G(char *str);
Node *get_T();
Node *get_P();
Node *get_N();
Node *get_E();
Node *get_W();
Node *get_F();
Node *get_V();
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

G ::= E#


E ::= T {[+ -]T}*

T ::= W {[/ *]W}* 

W :: = F{^F}?


F ::= cosP | sinP | P


P ::= (E) | N | V | F

V ::= [A-Z]

N ::= [0-9]+

*/

char *s = NULL;

Node *get_G(char *str){
	
	s = str;

	Node *val = get_E();
	assert(*s == '\0');

	return val;
}

Node *get_F(){
	Node *val1 = NULL;

	if((*s == 's' && *(s+1) == 'i' && *(s+2) == 'n') ||
		(*s == 'c' && *(s+1) == 'o' && *(s+2) == 's')){
		char op = *s;
		s += 3;
		Node *val2 = get_P();
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
	else
		val1 = get_P();

	return val1;
}

Node *get_E(){

	Node *val1 = get_T();
	Node *val2 = NULL;
	Node *new_node = NULL;

	while(*s == '+' || *s == '-'){
		char op = *s;
		s++;
		val2 = get_T();

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

Node *get_T(){
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
	Node *val1 = get_F();

	if(*s == '^'){
		s++;
		Node *val2 = get_F();		
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
		Node *val = get_E();		
		assert(*s == ')');
		s++;
		return val;
	}
	else if('0' <= *s && *s <= '9')
		return get_N();
	else if('A' <= *s && *s <= 'Z') 
		return get_V();
	else 
		return get_F();
}

Node *get_V(){
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

Node *get_N(){
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

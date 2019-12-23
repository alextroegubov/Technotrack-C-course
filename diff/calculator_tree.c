#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "tree.h"
#include <ctype.h>

Node *get_G(char *str);
Node *get_T();
Node *get_P();
Node *get_number();
Node *get_E();
Node *get_power();
Node *get_function();
Node *get_var();

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

T ::= power {[/ *]power}* 

power ::= P{^P}?

P ::= (E) | number | var | function

function ::= cos(E) | sin(E) | ln(E) | exp(E)

						exp ::= P^P
						power ::= P^number

var ::= [A-Z]

number ::= [0-9]+

*/

char *s = NULL;

Node *get_G(char *str){
	
	s = str;

	Node *val = get_E();
	assert(*s == '\0');

	return val;
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
	Node *val1 = get_power();
	Node *new_node = NULL;

	while(*s == '*' || *s == '/'){		
		char op = *s;
		s++;
		Node *val2 = get_power();

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


Node *get_P(){
	
	if(*s == '('){		
		s++;
		Node *val = get_E();		
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

Node *get_function(){
	Node *val1 = NULL;

	if((*s == 's' && *(s+1) == 'i' && *(s+2) == 'n' && *(s+3) == '(') ||
		(*s == 'c' && *(s+1) == 'o' && *(s+2) == 's' && *(s+3) == '(') ||
		(*s == 'e' && *(s+1) == 'x' && *(s+2) == 'p' && *(s+3) == '(')){
		char op = *s;
		s += 4;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;

		if(op == 's'){
			val1 = create_node(FUNC);
			val1->left = val2;
			val1->right = NULL;
			((Info_func*)(val1->info))->func = SIN;
		}
		else if(op == 'c'){
			val1 = create_node(FUNC);
			val1->left = val2;
			val1->right = NULL;
			((Info_func*)(val1->info))->func = COS;
		}
		else{
			val1 = create_node(FUNC);
			val1->left = val2;
			val1->right = NULL;
			((Info_func*)(val1->info))->func = EXP;
		}

	}
	else if( *s == 'l' && *(s+1) == 'n' && *(s+2) == '('){
		char op = *s;
		s += 3;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;

		if(op == 'l'){
			val1 = create_node(FUNC);
			val1->left = val2;
			val1->right = NULL;
			((Info_func*)(val1->info))->func = LN;
		}
	}
	else
		;//		val1 = get_power(); 

	return val1;
}

Node *get_power(){
	Node *val1 = get_P();
	if(*s == '^'){
		s++;
		Node *val2 = get_P();
		Node *new_node = create_node(FUNC);
		new_node->left = val1;
		new_node->right = val2;
		((Info_func*)(new_node->info))->func = POWER;
		val1 = new_node;
	}

	return val1;
}


Node *get_var(){
	char val = 0;
	Node *new_node = NULL;

	if('A' <= *s && *s <= 'Z'){
		val = tolower(*s);
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

//	diff_tech_print(tree);

	Tree *tree_diff = diff_tree(tree, 'x');
	tree_save_graph(tree_diff, "tree_diff.dot");

//	diff_tech_print(tree_diff);


	return 0;
}

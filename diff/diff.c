#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tree.h"
#include <ctype.h>
#include <string.h>
#include "diff.h"

//DSL
#define CP(node) copy_node(node)
#define DF(node) diff_node(node)
#define NL node->left
#define NR node->right

Node *_ADD(Node *l_node, Node *r_node){
	assert(l_node);
	assert(r_node);	

	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = ADD; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_SUB(Node *l_node, Node *r_node){
	assert(l_node);
	assert(r_node);	

	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = SUB; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_MUL(Node *l_node, Node *r_node){
	assert(l_node);
	assert(r_node);	

	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = MUL; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_DIV(Node *l_node, Node *r_node){
	assert(l_node);
	assert(r_node);	
		
	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = DIV; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_POWER(Node *arg_l, Node *arg_r){
	assert(arg_l);
	assert(arg_r);	
	
	Node *new_node = create_node(FUNC);
	((Info_func*)((new_node)->info))->func = POWER;
	new_node->left = copy_node(arg_l);
	new_node->right = copy_node(arg_r);

	return new_node;
}

Node *_FUNC(enum func func_name, Node *arg){
	assert(arg);

	Node *new_node = create_node(FUNC);
	((Info_func*)((new_node)->info))->func = func_name;
	new_node->left = copy_node(arg);

	return new_node;
}

Node *_NUM(double num){

	Node *new_node = create_node(NUM);
	((Info_num*)((new_node)->info))->num = (int)(num * FRAC_SZ);
	printf("num = %d\n", ((Info_num*)((new_node)->info))->num);

	return new_node;
}

Node *_VAR(char var){

	Node *new_node  = create_node(VAR);
	((Info_var*)(new_node->info))->var = var;

	return new_node;
}


char *s = NULL;

Node *get_G(char *str){
	assert(str);
		
	s = str;
	Node *val = get_E();
	assert(*s == '\0');

	return val;
}

Node *get_E(){

	Node *val1 = get_T();
	Node *val2 = NULL;

	while(*s == '+' || *s == '-'){
		char op = *s;
		s++;
		val2 = get_T();
		val1 = (op == '+')? _ADD(val1, val2) : _SUB(val1, val2);
	}

	return val1;
}

Node *get_T(){

	Node *val1 = get_power();

	while(*s == '*' || *s == '/'){		
		char op = *s;
		s++;
		Node *val2 = get_power();
		val1 = (op == '*')? _MUL(val1, val2) : _DIV(val1, val2);
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
	else if((*s == '-' && isdigit(*(s+1))) || isdigit(*(s)))
		return get_number();
	else if('A' <= *s && *s <= 'Z') 
		return get_var();
	else 
		return get_function();
}

Node *get_function(){

	Node *val1 = NULL;
	char str[100] = "";
	int i = 0;

	while(isalpha(*(s+i))){
		str[i] = *(s+i);
		i++;
	}
	str[i] = *(s+i);

	if(strcmp(str, "sin(") == 0 || strcmp(str, "cos(") == 0 || strcmp(str, "exp(") == 0){
		char op = *s;
		s += 4;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;
	
		if(op == 's')
			val1 = _FUNC(SIN, val2);			
		else if(op == 'c')
			val1 = _FUNC(COS, val2);
		else
			val1 = _FUNC(EXP, val2);

	}
	else if(strcmp(str, "ln(") == 0 || strcmp(str, "tg(") == 0 
			|| strcmp(str, "sh(") == 0 || strcmp(str, "ch(") == 0){
		char op = *s;
		s += 3;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;

		if(op == 's')
			val1 = _FUNC(SH, val2);
		else if(op == 'c')
			val1 = _FUNC(CH, val2);
		else if(op == 'l')
			val1 = _FUNC(LN, val2);
		else 
			val1 = _FUNC(TG, val2);
	}
	else if(strcmp(str, "arcsin(") == 0 || strcmp(str, "arccos(") == 0){
		char op = *(s+3);
		s += 7;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;

		val1 = _FUNC((op == 's')? ARCSIN : ARCCOS, val2);
	}
	else if(strcmp(str, "arctg(") == 0){
		s += 6;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;

		val1 = _FUNC(ARCTG, val2);
	}
	else if(strcmp(str, "sqrt(") == 0){
		s += 5;
		Node *val2 = get_E();
		assert(*s == ')');
		s++;

		val1 = _FUNC(SQRT, val2);
	}
	
	return val1;
}

Node *get_power(){

	Node *val1 = get_P();

	if(*s == '^'){
		s++;
		Node *val2 = get_P();
		
		val1 = _POWER(val1, val2);
	}

	return val1;
}

Node *get_var(){

	char val = 0;
	Node *new_node = NULL;

	if('A' <= *s && *s <= 'Z'){
		val = tolower(*s);
		s++;
		new_node = _VAR(val);
	}

	return new_node;
}

Node *get_number(){

	char *endptr = NULL;

	double val = strtod(s, &endptr);

	s = s + (endptr - s);

	Node *new_node = _NUM(val); 

	return new_node;
}

char _diff_var_ = 'x';

Tree *diff_tree(Tree *tree, char var){
	assert(tree);

	Tree *new_tree = calloc(1, sizeof(Tree));

	_diff_var_ = var;	
	
	new_tree->root = diff_node(tree->root);

	return new_tree;
}

Node *diff_node(Node *node){
	assert(node);

	switch(node->type){
		case NUM:
			return _NUM(0);

		case FUNC:
			return diff_node_func(node);

		case VAR:
			return _NUM(((Info_var*)(node->info))->var == _diff_var_ ? 1 : 0);

		case OP:
			return diff_node_op(node);
	}
}

Node *diff_node_op(Node *node){
	assert(node);

	switch(((Info_op*)(node->info))->op){

		case ADD:
			return _ADD(DF(NL), DF(NR));

		case SUB:
			return _SUB(DF(NL), DF(NR));

		case MUL:
			return _ADD(_MUL(DF(NR), CP(NL)), _MUL(CP(NR), DF(NL)));

		case DIV:
			if(NR->type == NUM)
				return _DIV(DF(NL),CP(NR));

			return _DIV(_SUB(_MUL(DF(NL), CP(NR)), _MUL(CP(NL), DF(NR))), _MUL(CP(NR), CP(NR)));
	}
}

Node *diff_node_func(Node *node){
	assert(node);
	
	int tmp_num = 0;

	switch(((Info_func*)(node->info))->func){
		case COS:
			return _MUL(_NUM(-1), _MUL(_FUNC(SIN, NL), DF(NL)));

		case SIN:
			return _MUL(_FUNC(COS, NL), DF(NL));

		case POWER:
			if(NL->type == VAR && NR->type == NUM){
				if(((Info_var*)(NL->info))->var == _diff_var_){
					tmp_num = ((Info_num*)(NR->info))->num;
					return _MUL(_NUM(tmp_num), _POWER(CP(NL), _NUM(tmp_num - 1)));
				}
				else
					return _NUM(0);
			}
			else if(NR->type == NUM){
				tmp_num = ((Info_num*)(NR->info))->num;
				return _MUL(_NUM(tmp_num), _MUL(DF(NL), _POWER(CP(NL), _NUM(tmp_num - 1))));
			}
			else
				//f^g = exp(g*ln(f))  
				return  DF(_FUNC(EXP, _MUL(CP(NR), _FUNC(LN, CP(NL)))));		

		case LN:
			return _MUL(DF(NL), _DIV(_NUM(1), CP(NL))); 
	
		case EXP:
			return _MUL(DF(NL), CP(node));

		case SH:
			return _MUL(DF(NL), _FUNC(CH, CP(NL)));

		case CH:
			return _MUL(DF(NL), _FUNC(SH, CP(NL)));

		case TG:
			return _MUL(DF(NL), _DIV(_NUM(1), _MUL(_FUNC(COS, CP(NL)), _FUNC(COS, CP(NL)))));
		
		case ARCTG:
			return _MUL(DF(NL), _DIV(_NUM(1), _ADD(_NUM(1), _MUL(CP(NL), CP(NL)))));
		
		case SQRT:
			return _MUL(DF(NL), _DIV(_NUM(1), _MUL(_NUM(2), _FUNC(SQRT, NL))));

		case ARCSIN:
			return _MUL(DF(NL), _DIV(_NUM(1), _FUNC(SQRT, _ADD(_NUM(1), _POWER(NL, _NUM(2))))));
		
		case ARCCOS:
			return _MUL(_MUL(DF(NL), _NUM(-1)), _DIV(_NUM(1), _FUNC(SQRT, _ADD(_NUM(1), _POWER(NL, _NUM(2))))));

		default:
			printf("Error: diff_node_func: unknown function!\n");
	}
}

Node *copy_node(Node *node){
	assert(node);
	
	Node *new_node = create_node(node->type);

	switch(node->type){
		case OP:
			((Info_op*)(new_node->info))->op = ((Info_op*)(node->info))->op;
			break;
		case FUNC:
			((Info_func*)(new_node->info))->func = ((Info_func*)(node->info))->func;
			break;
		case NUM:
			((Info_num*)(new_node->info))->num = ((Info_num*)(node->info))->num;
			break;
		case VAR:
			((Info_var*)(new_node->info))->var = ((Info_var*)(node->info))->var;
			break;
		default:
			printf("Error: copy_node: unknown type!\n");
	}
	
	if(node->right)
		new_node->right = copy_node(node->right);
	
	if(node->left)
		new_node->left = copy_node(node->left);
	
	return new_node;
}

char _simple_tree_ = 1;

Node *simplify_node_div(Node *node){
	assert(node);
	assert(node->type == OP);

	double num1 = 0;
	double num2 = 0;
	Node *new_node = node;

	if(NL->type == NUM && NR->type == NUM){
		num1 = ((Info_num*)(NL->info))->num / FRAC_SZ;
		num2 = ((Info_num*)(NR->info))->num / FRAC_SZ;
		tree_delete_subtree(node);
		_simple_tree_ = 0;

		return _NUM(num1 / num2);
	}
	else if(NL->type == NUM){
		num1 = ((Info_num*)(NL->info))->num / FRAC_SZ;

		if(num1 == 0.0){
			tree_delete_subtree(node);
			_simple_tree_ = 0;
			return _NUM(0);
		}
	}
	else if(NR->type == NUM){
		num2 = ((Info_num*)(NR->info))->num / FRAC_SZ;

		if(num2 == 1.0){
			tree_free_node(NR);
			new_node = NL;
			tree_free_node(node);
			_simple_tree_ = 0;
		}
	}
	
	return new_node;
}

Node *simplify_node_mul(Node *node){
	assert(node);
	assert(node->type == OP);

	double num1 = 0;
	double num2 = 0;
	Node *new_node = node;

	if(NL->type == NUM && NR->type == NUM){
		num1 = ((Info_num*)(NL->info))->num / FRAC_SZ;
		num2 = ((Info_num*)(NR->info))->num / FRAC_SZ;
		tree_delete_subtree(node);
		_simple_tree_ = 0;

		return _NUM(num1 * num2);
	}
	else if(NL->type == NUM){
		num1 = ((Info_num*)(NL->info))->num / FRAC_SZ;

		if(num1 == 0.0){
			tree_delete_subtree(node);
			_simple_tree_ = 0;
			return _NUM(0);
		}
		else if(num1 == 1.0){
			tree_free_node(NL);
			new_node = NR;
			tree_free_node(node);
			_simple_tree_ = 0;
		}
	}
	else if(NR->type == NUM){
		num2 = ((Info_num*)(NR->info))->num / FRAC_SZ;

		if(num2 == 0.0){
			tree_delete_subtree(node);
			_simple_tree_ = 0;
			return _NUM(0);
		}
		else if(num2 == 1.0){
			tree_free_node(NR);
			new_node = NL;
			tree_free_node(node);
			_simple_tree_ = 0;
		}
	}
	
	return new_node;
}


Node *simplify_node_add_sub(Node *node){
	assert(node);
	assert(node->type == OP);

	double num1 = 0;
	double num2 = 0;
	Node *new_node = node;

	enum op oper = ((Info_op*)(node->info))->op;

	if(NL->type == NUM && NR->type == NUM){
		num1 = ((Info_num*)(NL->info))->num / FRAC_SZ;
		num2 = ((Info_num*)(NR->info))->num / FRAC_SZ;
		tree_delete_subtree(node);

		_simple_tree_ = 0;

		return _NUM((oper == ADD) ? num1 + num2 : num1 - num2);
	}
	else if(NL->type == NUM && ((Info_num*)(NL->info))->num == 0.0){
		tree_free_node(NL);
		new_node = (oper == ADD) ? NR : _MUL(_NUM(-1), NR);
		tree_free_node(node);

		_simple_tree_ = 0; 
	}
	else if(NR->type == NUM && ((Info_num*)(NR->info))->num == 0.0){
		tree_free_node(NR);
		new_node = NL;
		tree_free_node(node);

		_simple_tree_ = 0;
	}

	return new_node;
}

Node *simplify_node(Node *node){
	assert(node);

	Node *new_node = node;

	if(NL) 
		NL = simplify_node(NL);
	if(NR) 
		NR = simplify_node(NR);

	if(node->type == OP){
		switch(((Info_op*)(node->info))->op){
			case ADD:
			case SUB:
				 new_node = simplify_node_add_sub(node);
				 break;

			case DIV:
				new_node = simplify_node_div(node);
				break;

			case MUL:
				new_node = simplify_node_mul(node);
				break;
		}
	}
	else if(node->type == FUNC && NL->type == NUM){
		double num = ((Info_num*)(NL->info))->num;
		
		//free_node after func
		switch(((Info_func*)(node->info))->func){
			case COS:
				return _NUM(cos(num));

			case SIN:
				return _NUM(sin(num));

			case POWER: //fix me: move power to operations
				if(NR->type == NUM){
					double num2 = ((Info_num*)(NR->info))->num;
					return _NUM(pow(num, num2));
				}
				return node;
			case LN:
				return _NUM(log(num)); 
	
			case EXP:
				return _NUM(exp(num));

			case SH:
				return _NUM((exp(num) - exp(-num))/2);

			case CH:
				return _NUM((exp(num) + exp(-num))/2);

			case TG:
				return _NUM(tan(num));

			case ARCTG:
				return _NUM(atan(num));

			case SQRT:
				return _NUM(sqrt(num));

			case ARCSIN:
				return _NUM(asin(num));
		
			case ARCCOS:
				return _NUM(acos(num));

			default:
				printf("Error: diff_node_func: unknown function!\n");
		}
	}
	return new_node;
}

Tree *simplify_tree(Tree *tree){
	assert(tree);
	
	_simple_tree_ = 0;

	while(!_simple_tree_){
		_simple_tree_ = 1;
		tree->root = simplify_node(tree->root);
	}

	return tree;
}


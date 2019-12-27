#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tree.h"

Tree *tree_create(){
	Tree *tree = calloc(1, sizeof(Tree));
	if(!tree){
		printf("Error: tree_create: can't create tree!");
		return NULL;
	}

	return tree;
}

//creates empty node of requested type
Node *create_node(enum type type){
	
	Node *new_node = calloc(1, sizeof(Node));

	if(!new_node){
		printf("Error: create_node: can't create node!\n");
		return NULL;
	}
	new_node->type = type;

	switch(type){
		case OP:
			new_node->info = calloc(1, sizeof(Info_op));
			break;
		case FUNC:
			new_node->info = calloc(1, sizeof(Info_func));
			break;
		case NUM:
			new_node->info = calloc(1, sizeof(Info_num));
			break;
		case VAR:
			new_node->info = calloc(1, sizeof(Info_var));
			break;
		default:
			printf("%d", type); 
			printf("Error: create_node: unknown type\n");
	}

	return new_node;
}


void tree_visitor_pre_order(Node *node, int(*function)(Node *)){
	assert(node);
	assert(function);

	(*function)(node);
	
	if(node->left)
		tree_visitor_pre_order(node->left, function);

	if(node->right)
		tree_visitor_pre_order(node->right, function);
}

void tree_visitor_in_order(Node *node, int(*function)(Node *)){
	assert(node);
	assert(function);
	
	if(node->left)
		tree_visitor_in_order(node->left, function);

	(*function)(node);

	if(node->right)
		tree_visitor_in_order(node->right, function);
}

void tree_visitor_post_order(Node *node, int(*function)(Node *)){
	assert(node);
	assert(function);
	
	if(node->left)
		tree_visitor_post_order(node->left, function);

	if(node->right)
		tree_visitor_post_order(node->right, function);

	(*function)(node);
}

void tree_destroy(Tree *tree){
	assert(tree);
	
	if(tree->root)
		tree_visitor_post_order(tree->root, tree_free_node);

	free(tree);

	tree = NULL;
}

int tree_delete_subtree(Node *node){
	assert(node);

	tree_visitor_post_order(node, tree_free_node);
	
	return 0;
}

int tree_free_node(Node *node){
	assert(node);

	node->left = NULL;
	node->right = NULL;
	free(node->info);
	free(node);

	return 0;
}


static FILE *_tree_file_ = NULL;

int tree_save_graph(const Tree *tree, const char *tree_image){
	assert(tree);
	assert(tree_image);

	_tree_file_ = fopen(tree_image, "w");

	if(!_tree_file_){
		printf("Error: tree_save_graph:\n");
		exit(-3);
	}
	fprintf(_tree_file_, "digraph G {\n");
	
	tree_visitor_in_order(tree->root, tree_print_node_graph);
	
	fprintf(_tree_file_, "\n");
	fprintf(_tree_file_, "}");
	fclose(_tree_file_);

	return 0;
}

#define PRINT_GR(NODE, STR) \
	fprintf(_tree_file_, "%lu[label = \" %s\"]\n", (unsigned long)NODE, STR); \
	break;

int tree_print_node_graph(Node *node){
	assert(node);

	if(!_tree_file_){
		printf("Error: tree_print_node_graph:\n");
		exit(-10);
	}

	switch(node->type){
		case OP:
			switch(((Info_op*)(node->info))->op){
				case ADD:
					PRINT_GR(node, "+");
				case SUB:
					PRINT_GR(node, "-");
				case MUL:
					PRINT_GR(node, "*");
				case DIV:
					PRINT_GR(node,"/");
			};
			break;

		case FUNC:
			switch(((Info_func*)(node->info))->func){
				case COS:
					PRINT_GR(node, "cos");
				case SIN:
					PRINT_GR(node, "sin");
				case POWER:
					PRINT_GR(node, "^");
				case LN:
					PRINT_GR(node, "ln");
				case EXP:
					PRINT_GR(node, "exp");
				case SH:
					PRINT_GR(node, "sh");
				case CH:
					PRINT_GR(node, "ch");
				case TG:
					PRINT_GR(node, "tg");			
				case ARCTG:
					PRINT_GR(node, "arctg");
				case ARCSIN:
					PRINT_GR(node, "arcsin");
				case ARCCOS:
					PRINT_GR(node, "arccos");
			};
			break;

		case NUM:
			fprintf(_tree_file_, "%lu[label = \"%d\"]\n", (unsigned long)node, ((Info_num*)(node->info))->num);
			break;
		case VAR:
			fprintf(_tree_file_, "%lu[label = \"%c\"]\n", (unsigned long)node, ((Info_var*)(node->info))->var);
			break;
	};

	if(node->left)
		fprintf(_tree_file_, "%lu:sw ->%lu;\n", (unsigned long)node, (unsigned long)node->left);

	if(node->right)
		fprintf(_tree_file_, "%lu:se ->%lu;\n", (unsigned long)node, (unsigned long)node->right);

	return 0;
}

char _diff_var_ = 'x';

//DSL
#define DF(node) diff_node(node, _diff_var_)
#define NL node->left
#define NR node->right
#define CP(node) copy_node(node)

Node *_ADD(Node *l_node, Node *r_node){
	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = ADD; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_SUB(Node *l_node, Node *r_node){
	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = SUB; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_MUL(Node *l_node, Node *r_node){
	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = MUL; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_DIV(Node *l_node, Node *r_node){
	Node *new_node = create_node(OP); 
	((Info_op*)((new_node)->info))->op = DIV; 
	new_node->right = r_node; 
	new_node->left = l_node; 

	return new_node;
}

Node *_POWER(Node *arg_l, Node *arg_r){
	Node *new_node = create_node(FUNC);
	((Info_func*)((new_node)->info))->func = POWER;
	new_node->left = copy_node(arg_l);
	new_node->right = copy_node(arg_r);

	return new_node;
}

Node *_FUNC(enum func func_name, Node *arg){
	Node *new_node = create_node(FUNC);
	((Info_func*)((new_node)->info))->func = func_name;
	new_node->left = copy_node(arg);

	return new_node;
}

Node *_NUM(int num){
	Node *new_node = create_node(NUM);
	((Info_num*)((new_node)->info))->num = num;

	return new_node;
}

Node *_VAR(char var){
	Node *new_node  = create_node(VAR);
	((Info_var*)(new_node->info))->var = var;

	return new_node;
}


Tree *diff_tree(Tree *tree, char var){
	assert(tree);

	Tree *new_tree = calloc(1, sizeof(Tree));

	new_tree->root = diff_node(tree->root, var);

	return new_tree;
}

Node *diff_node(Node *node, char var){
	assert(node);

	switch(node->type){
		case NUM:
			return _NUM(0);

		case FUNC:
			return diff_node_func(node, var);

		case VAR:
			return _NUM(((Info_var*)(node->info))->var == var ? 1 : 0);

		case OP:
			return diff_node_op(node, var);
	}
}

Node *diff_node_op(Node *node, char var){
	assert(node);

	switch(((Info_op*)(node->info))->op){

		case ADD:
			return _ADD(DF(NL), DF(NR));

		case SUB:
			return _SUB(DF(NL), DF(NR));

		case MUL:
			return _ADD(_MUL(DF(NR), CP(NL)), _MUL(CP(NR), DF(NL)));

		case DIV:
			return _DIV(_SUB(_MUL(DF(NL), CP(NR)), _MUL(CP(NL), DF(NR))), _MUL(CP(NR), CP(NR)));
	}
}

Node *diff_node_func(Node *node, char var){
	assert(node);
	int tmp_num = 0;

	switch(((Info_func*)(node->info))->func){
		case COS:
			return _MUL(_NUM(-1), _MUL(_FUNC(SIN, NL), DF(NL)));

		case SIN:
			return _MUL(_FUNC(COS, NL), DF(NL));

		case POWER:
			if(NL->type == VAR && NR->type == NUM){
				if(((Info_var*)(NL->info))->var == var){
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
				//??allocating
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
		
		case ARCSIN: //sqrt??
			return NULL;
		
		case ARCCOS: //sqrt??
			return NULL;
	}
}

Node *copy_node(Node *node){
	//assert(node);
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
	}
	
	if(node->right)
		new_node->right = copy_node(node->right);
	
	if(node->left)
		new_node->left = copy_node(node->left);
	
	return new_node;
}

char _simple_tree_ = 1;

Node *simplify_node_mul(Node *node){
	assert(node);
	assert(node->type == OP);

	int num1 = 0;
	int num2 = 0;
	Node *new_node = node;

	if(NL->type == NUM && NR->type == NUM){
		num1 = ((Info_num*)(NL->info))->num;
		num2 = ((Info_num*)(NR->info))->num;
		tree_delete_subtree(node);
		_simple_tree_ = 0;
		return _NUM(num1 * num2);
	}
	else if(NL->type == NUM){
		num1 = ((Info_num*)(NL->info))->num;
		if(num1 == 0){
			tree_delete_subtree(node);
			_simple_tree_ = 0;
			return _NUM(0);
		}
		else if(num1 == 1){
			tree_free_node(NL);
			new_node = NR;
			tree_free_node(node);
			_simple_tree_ = 0;
		}
	}
	else if(NR->type == NUM){
		num2 = ((Info_num*)(NR->info))->num;
		if(num2 == 0){
			tree_delete_subtree(node);
			_simple_tree_ = 0;
			return _NUM(0);
		}
		else if(num2 == 1){
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

	int num1 = 0;
	int num2 = 0;
	Node *new_node = node;

	if(NL->type == NUM && NR->type == NUM){
		printf("*\n");
		num1 = ((Info_num*)(NL->info))->num;
		num2 = ((Info_num*)(NR->info))->num;
		tree_delete_subtree(node);
		_simple_tree_ = 0;
		return _NUM((((Info_op*)(node->info))->op == ADD) ? num1 + num2 : num1 - num2);
	}
	else if(NL->type == NUM && ((Info_num*)(NL->info))->num == 0){
		new_node = CP(NR);
		tree_delete_subtree(node);

		printf("**\n");/*
		tree_free_node(NL);
		new_node = NR;
		tree_free_node(node);
		*/
		_simple_tree_ = 0; 
	}
	else if(NR->type == NUM && ((Info_num*)(NR->info))->num == 0){
		new_node = CP(NL);
		tree_delete_subtree(node);

		printf("***\n");/*
		tree_free_node(NR);
		new_node = NL;
		tree_free_node(node);
		*/
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
				new_node = node;//simplify_node_div(node);
				break;

			case MUL:
				new_node = simplify_node_mul(node);
				break;
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


FILE* _file_tech_ = NULL;
#define TECH(A) \
	fprintf(_file_tech_, A)

void node_tech_print_func(Node *node){
	assert(node);
	
	switch(((Info_func*)(node->info))->func){
		case COS:
			TECH("\\cos(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case SIN:
			TECH("\\sin(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case LN:
			TECH("\\ln(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case EXP:
			TECH("\\exp(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case POWER:
			TECH("(");
			node_tech_print(node->left);
			TECH(")");
			TECH("^{");
			node_tech_print(node->right);
			TECH("}");
			break;
		case SH:
			TECH("\\sh(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case CH:
			TECH("\\ch(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case TG:
			TECH("\\tg(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case ARCCOS:
			TECH("\\arccos(");
			node_tech_print(node->left);
			TECH(")");
			break;
		case ARCSIN:
			TECH("\\arcsin(");
			node_tech_print(node->left);
			TECH(")");
			break;
	}
}

void node_tech_print_add(Node *node){
	assert(node);
	TECH("(");
	node_tech_print(node->right);
	TECH("+");
	node_tech_print(node->left);
	TECH(")");
}

void node_tech_print_sub(Node *node){
	assert(node);
	TECH("(");
	node_tech_print(node->right);
	TECH("-");
	node_tech_print(node->left);
	TECH(")");
}

void node_tech_print_mul(Node *node){
	assert(node);
	node_tech_print(node->right);
	TECH(" \\cdot ");
	node_tech_print(node->left);
}

void node_tech_print_div(Node *node){
	assert(node);
	TECH(" \\frac{");
	node_tech_print(node->left);
	TECH("}{");
	node_tech_print(node->right);
	TECH("}");
}

void node_tech_print_op(Node *node){
	assert(node);
	
	switch(((Info_op*)(node->info))->op){
		case ADD:
			node_tech_print_add(node);
			break;
		case SUB:
			node_tech_print_sub(node);
			break;
		case MUL:
			node_tech_print_mul(node);
			break;
		case DIV:
			node_tech_print_div(node);
			break;
	}
}

void node_tech_print(Node *node){
	assert(node);

	int num = 0;

	switch(node->type){
		case NUM:
			num = ((Info_num*)(node->info))->num;
			fprintf(_file_tech_, (num > 0) ? "%d" : "(%d)", num);
			break;
		case FUNC:
			node_tech_print_func(node);
			break;
		case VAR:
			fprintf(_file_tech_, "%c", tolower(((Info_var*)(node->info))->var));
			break;
		case OP:
			node_tech_print_op(node);
			break;
	}
}


////diff variable!!!!!!!!!!!!!!!!!!!!1

void diff_tech_print(Tree *tree, const char *filename){
	assert(tree);

	FILE *file = fopen(filename, "w");
	
	_file_tech_ = file;

	fprintf(file, 	"\\documentclass[a4paper]{article}\n"
					"\\usepackage[T2A]{fontenc}\n"
					"\\usepackage[utf8]{inputenc}\n"
					"\\usepackage[english,russian]{babel}\n"
					"\\usepackage{wrapfig}\n"
					"\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n"
					"\\usepackage[pdftex]{graphicx}\n"
					"\\graphicspath{{pictures/}}\n"
					"\\usepackage{floatflt}\n"
					"\\DeclareGraphicsExtensions{.pdf,.png,.jpg}\n"
					"\\usepackage[left=3cm,right=3cm,"
					"top=3.5cm,bottom=2cm,bindingoffset=0cm]{geometry}\n"
					"\\usepackage{wrapfig}\n"
					"\\usepackage{float}\n"
					"\\usepackage{graphicx}\n"
					"\\graphicspath{{pictures/}}\n\n"
					"\\begin{document}\n");
	
	TECH("$");
	node_tech_print(tree->root);
	TECH("$");

	fprintf(file, "\\end{document}\n");

	fclose(file);

//	system("pdflatex result/result.tex");
}
#undef TECH

/*
int tree_save(Tree *tree, const char *filename){
	assert(tree);
	assert(filename);

	_tree_file_ = fopen(filename, "w");

	_tree_save(tree->root);

	fclose(_tree_file_);
	return 0;
}
int _tree_save(Node *node){
	assert(node);

	fprintf(_tree_file_, "(%d", node->data);

	if(node->left)
		_tree_save(node->left);		
	else 
		fprintf(_tree_file_, "(#)");
	
	if(node->right)
		_tree_save(node->right);		
	else 
		fprintf(_tree_file_, "(#)");

	fprintf(_tree_file_, ")");
	return 0;
}
*//*
Node *tree_read_node(char *buffer, int pos){
	
	if(buffer[pos++] != '(')
		printf("Error:\n");
	
	if(buffer[pos] == '#'){
		pos += 2;//come over )
		return NULL;
	}
	else{
		Node *node = calloc(1, sizeof(Node));  
		int data = 0;
		sscanf(buffer[pos], "%d(", &data);
		pos = pos + strchr(&buffer[pos], '(') - &(buffer[pos]);

	}
}
*/
/*
Tree *tree_read_from_file(const char *filename){
	assert(filename);

	char *buffer = create_text_buffer(filename);
	//if

	Tree *tree = tree_read_node(buffer, pos);

	return NULL;
}
*/
/*
char *create_text_buffer(char *filename){
	assert(filename != NULL);

	int nsym = 0;
	FILE *file = fopen(filename, "r");

	if (fseek(file, 0, SEEK_END)){
		fclose(file);
		return NULL;
	}
	nsym = ftell(file); //number of sym in file
	fseek(file, 0, SEEK_SET);
	char *buffer = calloc(nsym + 1, sizeof(char));

	if (!buffer){ 
		fclose(file);
		return NULL;
	}
	buffer[*nsym] = '\n';	

	if(fread((void*)buffer, sizeof(char), nsym, file) != nsym){
		free(buffer);
		fclose(file);
		return NULL;
	}

	fclose(file);
	return buffer;
}
*//*
int main(){

	Tree *tree = tree_create("tree.log");
	
	Node *cur_node = tree->root;
	for(int i = 0; i <= 3;){
		cur_node->left = tree_create_node(cur_node, i++, 1);
		cur_node->right = tree_create_node(cur_node, i++, 1);
		cur_node = cur_node->left;
	}
	
	tree_visitor_in_order(tree->root, tree_print_node);

	tree_save_graph((const Tree *) tree, "tree.dot");

	tree_save(tree, "tree.txt");

	tree_destroy(tree);

	return 0;
}*/

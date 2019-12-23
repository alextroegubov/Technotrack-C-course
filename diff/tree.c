#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
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
	fprintf(_tree_file_, "%lu[label = \" %s\"]\n", (unsigned long)NODE, STR);

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
					break;
				case SUB:
					PRINT_GR(node, "-");
					break;
				case MUL:
					PRINT_GR(node, "*");
					break;
				case DIV:
					PRINT_GR(node,"/");
					break;
			};
			break;

		case FUNC:
			switch(((Info_func*)(node->info))->func){
				case COS:
					PRINT_GR(node, "cos");
					break;
				case SIN:
					PRINT_GR(node, "sin");
					break;
				case POWER:
					PRINT_GR(node, "^");
					break;
				case LN:
					PRINT_GR(node, "ln");
					break;
				case EXP:
					PRINT_GR(node, "exp");
					break;
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

#define _NUM(node, numb) \
	node = create_node(NUM); \
	((Info_num*)((node)->info))->num = numb;

#define _OP(node, oper) \
	node = create_node(OP); \
	((Info_op*)((node)->info))->op = oper;

#define _FUNC(node, funct) \
	node = create_node(FUNC); \
	((Info_func*)(node->info))->func = funct;


Tree *diff_tree(Tree *tree, char var){
	assert(tree);

	Tree *new_tree = calloc(1, sizeof(Tree));

	new_tree->root = diff_node(tree->root, var);

	return new_tree;
}

Node *diff_node(Node *node, char var){
	assert(node);
	Node *new_node = NULL;

	switch(node->type){
		case NUM:
			_NUM(new_node, 0);
			break;
		case FUNC:
			new_node = diff_node_func(node, var);
			break;
		case VAR:
			_NUM(new_node, ((Info_var*)(node->info))->var == var ? 1 : 0);
			break;
		case OP:
			printf("Entered OP\n");
			new_node = diff_node_op(node, var);
			break;
	}
	return new_node;
}

Node *diff_node_op(Node *node, char var){
	assert(node);
	Node *new_node  = NULL;
	Node *tmp_node = NULL;

	switch(((Info_op*)(node->info))->op){
		case ADD:
			_OP(new_node, ADD);
			new_node->right = diff_node(node->right, var);
			new_node->left = diff_node(node->left, var);
			break;
		case SUB:
			_OP(new_node, SUB);
			new_node->right = diff_node(node->right, var);
			new_node->left = diff_node(node->left, var);
			break;
		case MUL:
			_OP(new_node, ADD);
			assert(new_node);
			_OP((new_node->left), MUL);
			assert(new_node->left);
			new_node->left->left = diff_node(node->left, var);
			assert(new_node->left->left);
			new_node->left->right = copy_node(node->right);
			assert(new_node->left->right);

			_OP((new_node->right), MUL);
			assert(new_node->right);
			new_node->right->left = copy_node(node->left);
			assert(new_node->right->left);
			new_node->right->right = diff_node(node->right, var);
			break;
		case DIV:
			_OP(new_node, DIV);
			_OP(new_node->right, MUL);
			new_node->right->right = copy_node(node->right);
			new_node->right->left = copy_node(node->right);

			_OP(new_node->left, SUB);
			tmp_node = new_node->left;

			_OP(tmp_node->left, MUL);
			tmp_node->left->left = diff_node(node->left, var);
			tmp_node->left->right = copy_node(node->right);

			_OP(tmp_node->right, MUL);
			tmp_node->right->left = diff_node(node->right, var);
			tmp_node->right->right = copy_node(node->left);
			break;
	}

	return new_node;
}

Node *diff_node_func(Node *node, char var){
	assert(node);
	Node *new_node = NULL;
	_OP(new_node, MUL);
	Node *tmp_node = NULL;

	new_node->right = diff_node(node->left, var); //except power

	switch(((Info_func*)(node->info))->func){
		case COS:
			_OP(new_node->left, MUL);
			tmp_node = new_node->left;
			_NUM(tmp_node->left, -1);
			_FUNC(tmp_node->right, SIN);
			tmp_node->right->left = copy_node(node->left);
			break;
		case SIN:
			_FUNC(new_node->left, COS);
			new_node->left->left = copy_node(node->left);
			break;
		case POWER:
			///*//*****
			break;	
		case LN:
			_OP(new_node->left, DIV);
			_NUM(new_node->left->left, 1);
			new_node->left->right = copy_node(node->left);
			break;
		case EXP:
			_FUNC(new_node->left, EXP);
			new_node->left->left = copy_node(node->left);
			break;
	}

	return new_node;
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
#undef _NUM
#undef _OP
#undef _FUNC

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
			fprintf(_file_tech_, "\\sin(");
			node_tech_print(node->left);
			fprintf(_file_tech_, ")");
			break;
		case LN:
			fprintf(_file_tech_, "\\ln(");
			node_tech_print(node->left);
			fprintf(_file_tech_, ")");
			break;
		case EXP:
			fprintf(_file_tech_, "\\exp(");
			node_tech_print(node->left);
			fprintf(_file_tech_, ")");
			break;
		case POWER:
			TECH("(");
			node_tech_print(node->left);
			TECH(")");
			fprintf(_file_tech_, "^{");
			node_tech_print(node->right);
			fprintf(_file_tech_, "}");
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
	TECH("\\cdot");
	node_tech_print(node->left);
}

void node_tech_print_div(Node *node){
	assert(node);
	TECH("\\frac{");
	node_tech_print(node->right);
	TECH("}{");
	node_tech_print(node->left);
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

	switch(node->type){
		case NUM:
			fprintf(_file_tech_, "%d", ((Info_num*)(node->info))->num);
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

void diff_tech_print(Tree *tree){
	assert(tree);
	
	FILE *file = fopen("result/result.tex", "w");
	
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

	system("pdflatex result/result.tex");
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

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
				case POWER:
					PRINT_GR(node, "^");
			};
			break;

		case FUNC:
			switch(((Info_func*)(node->info))->func){
				case COS:
					PRINT_GR(node, "cos");
				case SIN:
					PRINT_GR(node, "sin");
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
				case SQRT:
					PRINT_GR(node, "sqrt");
			};
			break;

		case NUM:
			fprintf(_tree_file_, "%lu[label = \"%.2lf\"]\n", (unsigned long)node, ((Info_num*)(node->info))->num / FRAC_SZ);
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

#undef PRINT_GR


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

		case ARCTG: //check it
			TECH("\\arctg(");
			node_tech_print(node->left);
			TECH(")");
			break;

		case SQRT:
			TECH("\\sqrt{");
			node_tech_print(node->left);
			TECH("\\mathstrut}");
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

void node_tech_print_power(Node *node){
	assert(node);

		TECH("(");
		node_tech_print(node->left);
		TECH(")");
		TECH("^{");
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

		case POWER:
			node_tech_print_power(node);
			break;
	}
}

void node_tech_print(Node *node){
	assert(node);

	int num = 0;

	switch(node->type){
		case NUM:
			num = ((Info_num*)(node->info))->num;
			fprintf(_file_tech_, (num > 0) ? "%.2lf" : "(%.2lf)", num / FRAC_SZ);
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


void tree_tech_print(Tree *tree, const char *filename){
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
*/

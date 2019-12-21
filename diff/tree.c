#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "tree.h"
/*
Node_op *create_node_op(enum op data){
	Node_op *new_node = calloc(1, sizeof(Node_op));

	if(!new_node){
		printf("Error: create_node_op: can't create node!\n");
		return NULL;
	}	
	new_node->data = data;
	new_node->type = OP;

	return new_node;
}

Node_op *create_node_op(enum op data){
	Node_op *new_node = calloc(1, sizeof(Node_op));

	if(!new_node){
		printf("Error: create_node_op: can't create node!\n");
		return NULL;
	}	
	new_node->data = data;
	new_node->type = OP;

	return new_node;
}

Node_op *create_node_op(enum op data){
	Node_op *new_node = calloc(1, sizeof(Node_op));

	if(!new_node){
		printf("Error: create_node_op: can't create node!\n");
		return NULL;
	}	
	new_node->data = data;
	new_node->type = OP;

	return new_node;
}
*/

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
		default:
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
				case POW:
					PRINT_GR(node, "^");
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

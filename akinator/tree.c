#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

//creates without nodes//
Tree *tree_create(const char *log_file_name){
	Tree *tree = calloc(1, sizeof(Tree));
	if(!tree){
		exit(-1);
	}
//	tree->root = tree_create_node(tree->root, "root");
//	if(!tree->root){
//		exit(-2);
//	}
	
	//log_file
	//hash;
	return tree;
}

Node *tree_create_node(Node *parent, data_t value){
//	assert(parent);

	Node *new_node = calloc(1, sizeof(Node));

	if(!new_node){
		exit(-3);
	}	
	new_node->data = calloc(strlen(value) + 1, sizeof(char));
	strncpy(new_node->data, value, strlen(value));
	new_node->parent = parent;

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

	free(node->data);
	node->data = NULL;
	node->parent = NULL;
	node->left = NULL;
	node->right = NULL;
	free(node);
	node = NULL;

	return 0;
}


int tree_print_node(Node *node){
	assert(node);
	
	printf("node: data = %s\n", node->data);

	return 0;
}

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

int tree_print_node_graph(Node *node){
	assert(node);

	if(!_tree_file_){
		printf("Error: tree_print_node_graph:\n");
		exit(-10);
	}		
	fprintf(_tree_file_, "%lu[label = \"%s\"]\n", (unsigned long int)node, node->data);

	if(node->left)
		fprintf(_tree_file_, "%lu->%lu;\n", (unsigned long int)node, (unsigned long int)node->left);

	if(node->right)
		fprintf(_tree_file_, "%lu->%lu;\n", (unsigned long int)node, (unsigned long int)node->right);

	return 0;
}

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
	assert(_tree_file_);

	fprintf(_tree_file_, "(%s", node->data);

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

Node *tree_read_node(Node *parent, char *buffer, int *pos){
	assert(buffer);
	assert(pos);

	Node *new_node = NULL;

	if(buffer[(*pos)++] != '('){
		printf("Error: tree_read_node: wrong symbol");
		return NULL;
	}
	else if(buffer[*pos] == '#'){
		*pos += 2;
		return NULL;
	}
	else{ 
		char data[MAX_DATA_SIZE] = {'\0'};
		sscanf(buffer + *pos, "%[0-9 a-z A-Z _ . , ](", data);
		*pos = *pos + strlen(data);

		new_node = tree_create_node(parent, data);
		new_node->left = tree_read_node(new_node, buffer, pos);
		new_node->right = tree_read_node(new_node, buffer, pos);	
		(*pos)++;

		return new_node;
	}
}


int tree_read_from_file(Tree *tree, const char *filename){
	assert(filename);
	assert(tree);

	char *buffer = tree_create_text_buffer(filename);

	if(!buffer){
		printf("Error: tree_read_from_file\n");
		return 1;
	}
	int pos = 0;

	tree->root = tree_read_node(tree->root, buffer, &pos);
	//...........................^^.......and assert in create_node//
	free(buffer);
/*
	if(!tree->root){
		printf("Error: tree_read_from_file\n");
		return 2;
	}
*/
	return 0;
}

char *tree_create_text_buffer(const char *filename){
	assert(filename != NULL);

	int nsym = 0;
	FILE *file = fopen(filename, "r");

	if(!file) 
		return NULL;

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
	buffer[nsym] = '\n';	

	if(fread((void*)buffer, sizeof(char), nsym, file) != nsym){
		free(buffer);
		fclose(file);
		return NULL;
	}

	fclose(file);
	return buffer;
}


#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

Tree *tree_create(const char *log_file_name){
	Tree *tree = calloc(1, sizeof(Tree));
	if(!tree){
		exit(-1);
	}

	tree->root = tree_create_node(tree->root, "root");
	if(!tree->root){
		exit(-2);
	}
	
	//log_file
	//hash;
	return tree;
}

Node *tree_create_node(Node *parent, data_t value){
	Node *new_node = calloc(1, sizeof(Node));
	if(!new_node){
		exit(-3);
	}	
	new_node->data = calloc(strlen(value) + 1, sizeof(char));
	//if

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
	fprintf(_tree_file_, "%u[label = %s]\n", node, node->data);

	if(node->left)
		fprintf(_tree_file_, "%u->%u;\n", node, node->left);

	if(node->right)
		fprintf(_tree_file_, "%u->%u;\n", node, node->right);

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
	printf("new node, my pos = %d\n", *pos);//	
	Node *new_node = NULL;

	if(buffer[(*pos)++] != '(')
		return NULL;
	
	if(buffer[*pos] == '#'){
		printf("\t empty node\n");
		*pos += 2;//come over )
		return NULL;
	}
	else{ 
		char data[100] = {'\0'};
		sscanf(buffer + *pos, "%[0-9 a-z A-Z _ . , ](", data);
		*pos = *pos + strlen(data);
		printf("\t my data = %s\n", data);

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

	char *buffer = create_text_buffer(filename);
	//if

	int pos = 0;
	tree->root = tree_read_node(NULL, buffer, &pos);
	//if

	return 0;
}

char *create_text_buffer(const char *filename){
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
	buffer[nsym] = '\n';	

	if(fread((void*)buffer, sizeof(char), nsym, file) != nsym){
		free(buffer);
		fclose(file);
		return NULL;
	}

	fclose(file);
	return buffer;
}
/*
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
